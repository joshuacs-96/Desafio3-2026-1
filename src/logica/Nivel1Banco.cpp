#include "Nivel1Banco.h"

#include <QLineF>
#include <algorithm>
#include <cmath>

namespace {
constexpr double RADIO_ESCALERA = 15.0;
constexpr double RADIO_SALIDA = 30.0;
constexpr double RADIO_RECOGER = 22.0;

// Coordenadas lógicas. Todo se calcula a partir de las alturas de los pisos,
// así puerta, salida y objetos nunca quedan desalineados.
constexpr double PISO_Y[] = {305.597, 206.654, 106.458};  // piso 1 (abajo), piso 2, piso 3 (arriba)
constexpr double ESCALERA_X[] = {50.0, 230.0};       // un hueco de escalera a cada lado
constexpr double X_PUERTA = 270.0;                   // puerta de la bóveda, en el piso 3
constexpr double X_SALIDA = 375.0;                   // salida, en el piso 1

const QPointF INICIO_MINION(40.0, PISO_Y[0]);
const QPointF INICIO_GUARDIA(40.0, PISO_Y[2]);
const QPointF POSICION_PUERTA(X_PUERTA, PISO_Y[2]);
const QPointF POSICION_SOPORTE(250.0, PISO_Y[1]);
const QPointF POSICION_SALIDA(X_SALIDA, PISO_Y[0]);
}

Nivel1Banco::Nivel1Banco()
    : boveda(68.0, 3),
    camara(QPointF(265.0, 10.0), 180.0, 45.0, 1.5, 180.0),
    guardia(INICIO_GUARDIA, 260.0),
    botinRecolectado(0),
    radioCaptura(28.0),
    laserActivado(false){
    edificio.configurar({PISO_Y[0], PISO_Y[1], PISO_Y[2]}, {180.0, 320.0, 65.0, 380.0});
    guardia.establecerEdificio(&edificio);
    iniciar();
}

void Nivel1Banco::iniciar() {
    completado = false;
    perdido = false;
    laserActivado = false;
    botinRecolectado = 0;
    resistenciaSoporte = 40.0;
    ruidoGolpes = 0;
    tiempoDesdeGolpe = 0.0;
    tiempoBloqueoLaser = 0.0;
    muertePorLaser = false;
    lasersSuelo.clear();
    contactoLaserSuelo = false;
    risasEmitidas = 0;

    minion.establecerPosicion(INICIO_MINION);

    // las zonas del mapa de calor son los pisos
    guardia.registrarZona("Piso 1", QPointF(200.0, edificio.alturaDelPiso(0)));
    guardia.registrarZona("Piso 2", QPointF(200.0, edificio.alturaDelPiso(1)));
    guardia.registrarZona("Piso 3", QPointF(200.0, edificio.alturaDelPiso(2)));
    guardia.registrarZona("Piso 1 izquierda", QPointF(45.0, edificio.alturaDelPiso(0)));
    guardia.registrarZona("Piso 2 izquierda", QPointF(45.0, edificio.alturaDelPiso(1)));

    items.clear();

    // Botín dentro de la bóveda (piso 3, a la derecha de la puerta): se activa al abrirla
    items.append(ItemRecolectable("Lingote", QPointF(300.0, PISO_Y[2]), 1, true));
    items.append(ItemRecolectable("Bolsa",   QPointF(325.0, PISO_Y[2]), 2, true));
    items.append(ItemRecolectable("Lingote", QPointF(350.0, PISO_Y[2]), 1, true));
    items.append(ItemRecolectable("Bolsa",   QPointF(375.0, PISO_Y[2]), 2, true));

    // Bananas repartidas por los pisos de abajo (activan el modo fortachón)
    items.append(ItemRecolectable("Banana", QPointF(150.0, PISO_Y[0]), 0, false));
    items.append(ItemRecolectable("Banana", QPointF(120.0, PISO_Y[1]), 0, false));
}

void Nivel1Banco::actualizar(double deltaTiempo) {
    if (completado || perdido) return;

    minion.actualizar(deltaTiempo);
    tiempoBloqueoLaser = std::max(0.0, tiempoBloqueoLaser - deltaTiempo);
    if (ruidoGolpes > 0) {
        tiempoDesdeGolpe += deltaTiempo;
        if (tiempoDesdeGolpe >= 1.5) {
            ruidoGolpes = 0;
        }
    }

    verificarLaser();
    if (perdido) return;

    camara.actualizar(deltaTiempo);
    recogerItemsCercanos();

    if (minion.hayRisaPendiente()) {
        ++risasEmitidas;
        procesarRuido(minion.generarRisaAutomatica());
    }

    if (camara.detecta(minion)) {
        Ruido alarma {"Alarma de camara", minion.obtenerPosicion(), 10.0, 1.0};
        procesarRuido(alarma);
    }

    guardia.ver(minion.obtenerPosicionSuelo(), minion.estaEnEscalera(),
                minion.obtenerDestinoEscalera());
    guardia.decidir();

    QString estadoAntesDeActuar = guardia.obtenerEstadoActual();
    edificio.actualizarLasers(!soporteDestruido(), !boveda.estaAbierta());
    guardia.actuar(deltaTiempo);

    // La captura mide la distancia al SUELO: saltar no sirve para pasar sobre el guardia.
    QLineF distancia(guardia.obtenerPosicion(), minion.obtenerPosicionSuelo());

    if (estadoAntesDeActuar == "Investigar"
        && guardia.obtenerEstadoActual() == "Patrullar"
        && distancia.length() > radioCaptura) {
        guardia.aprender(determinarZona(guardia.obtenerPosicion()), false);   // falsa alarma
    }

    verificarCaptura();
    verificarObjetivo();
}

void Nivel1Banco::moverMinion(const QPointF& direccion, double deltaTiempo) {
    if (completado || perdido || tiempoBloqueoLaser > 0.0) {
        return;
    }

    const QPointF posicionAnterior = minion.obtenerPosicionSuelo();
    minion.mover(direccion, deltaTiempo);
    const QPointF posicionActual = minion.obtenerPosicionSuelo();
    if (!boveda.estaAbierta() && edificio.pisoDe(posicionAnterior) == 2
        && posicionAnterior.x() <= X_PUERTA && posicionActual.x() > X_PUERTA) {
        minion.establecerPosicion(QPointF(X_PUERTA, PISO_Y[2]));
    }
    if (!direccion.isNull() && deltaTiempo > 0.0) {
        guardia.activarMovimiento();
    }
    minion.limitarHorizontal(10.0, 390.0);
}

void Nivel1Banco::saltarMinion() {
    if (!completado && !perdido && tiempoBloqueoLaser <= 0.0) {
        minion.saltar();
        guardia.activarMovimiento();
    }
}

bool Nivel1Banco::usarEscalera(bool subir) {
    if (completado || perdido || tiempoBloqueoLaser > 0.0 || minion.estaEnEscalera()) {
        return false;
    }

    for (const Escalera& escalera : edificio.escaleras()) {
        const QPointF origen = subir
                                   ? escalera.obtenerPosicion()
                                   : escalera.obtenerDestino();

        const QPointF destino = subir
                                    ? escalera.obtenerDestino()
                                    : escalera.obtenerPosicion();

        // Solo permite subir si el destino está más arriba.
        if (subir && destino.y() >= origen.y()) {
            continue;
        }

        // Solo permite bajar si el destino está más abajo.
        if (!subir && destino.y() <= origen.y()) {
            continue;
        }

        QLineF distancia(minion.obtenerPosicionSuelo(), origen);

        if (distancia.length() <= RADIO_ESCALERA) {
            minion.iniciarEscalera(destino);
            guardia.activarMovimiento();
            return true;
        }
    }

    return false;
}

void Nivel1Banco::danarPuerta(double dano) {
    if (completado || perdido || dano <= 0.0) {
        return;
    }

    bool estabaAbierta = boveda.estaAbierta();
    boveda.recibirDanio(dano);
    if (!estabaAbierta && boveda.estaAbierta()) activarAlarmaBoveda();

    Ruido golpe {"Golpe a la puerta", minion.obtenerPosicion(), 7.0, 1.0};
    procesarRuido(golpe);
}

bool Nivel1Banco::cercaDeObjetivo() const {
    QPointF posicion = minion.obtenerPosicionSuelo();
    return (!soporteDestruido() && QLineF(posicion, POSICION_SOPORTE).length() <= 30.0)
           || (soporteDestruido() && !boveda.estaAbierta()
               && QLineF(posicion, POSICION_PUERTA).length() <= 30.0);
}

void Nivel1Banco::golpear() {
    if (completado || perdido || tiempoBloqueoLaser > 0.0 || !cercaDeObjetivo()) return;
    double dano = minion.tieneBananaActiva() ? 20.0 : 10.0;
    if (!soporteDestruido()
        && QLineF(minion.obtenerPosicionSuelo(), POSICION_SOPORTE).length() <= 30.0) {
        resistenciaSoporte = std::max(0.0, resistenciaSoporte - dano);
    } else {
        boveda.recibirDanio(dano);
        if (boveda.estaAbierta()) activarAlarmaBoveda();
    }
    ++ruidoGolpes;
    tiempoDesdeGolpe = 0.0;
    if (ruidoGolpes >= 3) {
        Ruido ruido {"Golpes", minion.obtenerPosicionSuelo(), 7.0, 1.0};
        procesarRuido(ruido);
    }
}

void Nivel1Banco::activarAlarmaBoveda() {
    for (int piso = 0; piso < 3; ++piso) {
        if (piso == 0) {
            // Rayos separados para que el jugador deba saltar en el camino a la salida.
            lasersSuelo.append(QPointF(235.0, PISO_Y[piso]));
            lasersSuelo.append(QPointF(295.0, PISO_Y[piso]));
            lasersSuelo.append(QPointF(340.0, PISO_Y[piso]));
            continue;
        }
        if (piso == 1) {
            // Primer láser antes de la escalera de x=180; el otro queda hacia el soporte.
            lasersSuelo.append(QPointF(150.0, PISO_Y[piso]));
            lasersSuelo.append(QPointF(215.0, PISO_Y[piso]));
            continue;
        }
        if (piso == 2) {
            // Separados para que no aparezcan juntos al activarse la alarma.
            lasersSuelo.append(QPointF(105.0, PISO_Y[piso]));
            lasersSuelo.append(QPointF(215.0, PISO_Y[piso]));
            continue;
        }
    }
    camara.activarAlarma();
    edificio.retirarEscaleraDerechaInferior();
    guardia.activarAlarma(POSICION_PUERTA);
}

bool Nivel1Banco::soporteDestruido() const { return resistenciaSoporte <= 0.0; }
bool Nivel1Banco::murioPorLaser() const { return muertePorLaser; }
int Nivel1Banco::obtenerRuidoGolpes() const { return ruidoGolpes; }
int Nivel1Banco::obtenerRisasEmitidas() const { return risasEmitidas; }
double Nivel1Banco::obtenerResistenciaSoporte() const { return resistenciaSoporte; }
QPointF Nivel1Banco::obtenerPosicionSoporte() const { return POSICION_SOPORTE; }
const QVector<QPointF>& Nivel1Banco::obtenerLasersSuelo() const { return lasersSuelo; }

const Minion& Nivel1Banco::obtenerMinion() const { return minion; }
const Boveda& Nivel1Banco::obtenerBoveda() const { return boveda; }
const CamaraSeguridad& Nivel1Banco::obtenerCamara() const { return camara; }
const GuardiaCentinela& Nivel1Banco::obtenerGuardia() const { return guardia; }
const QVector<ItemRecolectable>& Nivel1Banco::obtenerItems() const { return items; }
const Edificio& Nivel1Banco::obtenerEdificio() const { return edificio; }
int Nivel1Banco::obtenerBotinRecolectado() const { return botinRecolectado; }
QPointF Nivel1Banco::obtenerPosicionPuerta() const { return POSICION_PUERTA; }
QPointF Nivel1Banco::obtenerPosicionSalida() const { return POSICION_SALIDA; }

QString Nivel1Banco::obtenerZonaActual() const {
    return determinarZona(minion.obtenerPosicionSuelo());
}

bool Nivel1Banco::puedeEscapar() const {
    return boveda.estaAbierta() && botinRecolectado >= boveda.obtenerBotinMinimo();
}

void Nivel1Banco::procesarRuido(const Ruido& ruido) {
    guardia.percibir(ruido.posicion, ruido.intensidad);
}

void Nivel1Banco::verificarLaser() {
    if (minion.estaEnEscalera()) {
        laserActivado = false;
        contactoLaserSuelo = false;
        return;
    }

    const QPointF posicion = minion.obtenerPosicionSuelo();
    bool tocaSuelo = false;
    if (boveda.estaAbierta()) {
        const double altura = posicion.y() - minion.obtenerPosicion().y();
        for (const QPointF& laser : lasersSuelo) {
            if (std::abs(posicion.y() - laser.y()) < 8.0
                && std::abs(posicion.x() - laser.x()) <= 11.0 && altura < 12.0) {
                tocaSuelo = true;
                break;
            }
        }
        if (tocaSuelo && !contactoLaserSuelo) {
            const int vidasAntes = minion.obtenerVidas();
            minion.recibirDanio(40);
            if (minion.obtenerVidas() <= 0
                || (boveda.estaAbierta() && minion.obtenerVidas() < vidasAntes)) {
                muertePorLaser = true;
                perdido = true;
            }
            Ruido alarma {"Alarma de laser", posicion, 10.0, 1.0};
            procesarRuido(alarma);
        }
    }
    contactoLaserSuelo = tocaSuelo;
    const int piso = edificio.pisoDe(posicion);
    double laserX = -1.0;

    if (piso == 2 && !soporteDestruido() && std::abs(posicion.x() - 250.0) <= 5.0) {
        laserX = 250.0;
    } else if (!boveda.estaAbierta() && piso == 2 && std::abs(posicion.x() - 355.0) <= 5.0) {
        laserX = 355.0;
    } else if (!boveda.estaAbierta() && piso == 1 && std::abs(posicion.x() - 350.0) <= 5.0) {
        laserX = 350.0;
    }

    if (laserX < 0.0) {
        laserActivado = false;
        return;
    }

    if (!laserActivado) {
        laserActivado = true;
        const double lado = posicion.x() < laserX ? -1.0 : 1.0;
        minion.recibirDanio(40);
        if (minion.obtenerVidas() <= 0) {
            muertePorLaser = true;
            perdido = true;
        }
        minion.establecerPosicion(QPointF(laserX + lado * 22.0, posicion.y()));
        tiempoBloqueoLaser = 0.4;
        Ruido alarma {"Alarma de laser", posicion, 10.0, 1.0};
        procesarRuido(alarma);
    }
}

void Nivel1Banco::recogerItemsCercanos() {
    for (ItemRecolectable& item : items) {
        if (item.fueRecogido()) {
            continue;
        }
        if (item.estaDentroDeBoveda() && !boveda.estaAbierta()) {
            continue;                       // el botín no se alcanza con la puerta cerrada
        }

        // La banana exige mayor proximidad horizontal y tiene en cuenta el salto.
        if (item.obtenerTipo() == "Banana") {
            const QPointF diferencia = minion.obtenerPosicion() - item.obtenerPosicion();
            if (std::abs(diferencia.x()) > 8.0 || std::abs(diferencia.y()) > 12.0) {
                continue;
            }
        } else {
            QLineF distancia(minion.obtenerPosicion(), item.obtenerPosicion());
            if (distancia.length() > RADIO_RECOGER) {
                continue;
            }
        }

        item.recoger();

        if (item.obtenerTipo() == "Banana") {
            minion.comerBanana();
        } else {
            botinRecolectado += item.obtenerValor();
        }
    }
}

void Nivel1Banco::verificarCaptura() {
    if (perdido || completado) {
        return;
    }

    QLineF distancia(
        guardia.obtenerPosicion(),
        minion.obtenerPosicionSuelo()
        );

    if (distancia.length() > radioCaptura) {
        return;
    }

    // El daño se aplica una sola vez al iniciar la captura.
    guardia.aprender(determinarZona(minion.obtenerPosicionSuelo()), true);
    minion.recibirDanio(100);

    // Con la alarma de la bóveda activa, una captura termina el nivel.
    if (boveda.estaAbierta()) {
        perdido = true;
        return;
    }

    // Si ya no quedan vidas, el nivel queda perdido,
    // pero los personajes permanecen en el lugar de la captura.
    if (minion.obtenerVidas() <= 0) {
        perdido = true;
    }
}

void Nivel1Banco::verificarObjetivo() {
    if (!puedeEscapar()) {
        return;
    }

    // Hay que VOLVER a la salida del piso 1 con la puerta abierta y el botín
    QLineF alSalir(minion.obtenerPosicionSuelo(), POSICION_SALIDA);
    if (alSalir.length() <= RADIO_SALIDA) {
        completado = true;
    }
}

QString Nivel1Banco::determinarZona(const QPointF& posicion) const {
    return "Piso " + QString::number(edificio.pisoDe(posicion) + 1);
}

void Nivel1Banco::finalizarCaptura() {
    // Si todavía tiene vidas, reaparece en las posiciones iniciales.
    if (!perdido && minion.obtenerVidas() > 0) {
        minion.establecerPosicion(INICIO_MINION);
        guardia.reiniciar(INICIO_GUARDIA);
    }
}
