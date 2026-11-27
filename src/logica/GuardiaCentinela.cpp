#include "GuardiaCentinela.h"

#include <QLineF>
#include <QRandomGenerator>
#include <QtGlobal>
#include <algorithm>
#include <cmath>

#include "Edificio.h"

GuardiaCentinela::GuardiaCentinela(const QPointF& posicionInicial,
                                   double radioPercepcion)
    : posicion(posicionInicial),
    radioPercepcion(radioPercepcion),
    estadoActual("Patrullar"),
    ultimaPosicionRuido(posicionInicial),
    tasaAprendizaje(0.25),
    hayRuidoPendiente(false),
    velocidad(45.5),
    ultimaPosicionVista(posicionInicial) {
}

void GuardiaCentinela::establecerEdificio(const Edificio* nuevoEdificio) {
    edificio = nuevoEdificio;
}

void GuardiaCentinela::activarMovimiento() {
    movimientoActivado = true;
}

void GuardiaCentinela::activarAlarma(const QPointF& lugar) {
    alarmaActiva = true;
    ultimaPosicionRuido = lugar;
    hayRuidoPendiente = true;
    estadoActual = "Investigar";
    limpiarRutaConservandoEscalera();
    activarMovimiento();
}

void GuardiaCentinela::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
}

bool GuardiaCentinela::percibir(const QPointF& posicionRuido, double intensidad) {
    QLineF distancia(posicion, posicionRuido);

    double alcance = radioPercepcion;
    if (edificio) {
        int diferenciaPisos = std::abs(edificio->pisoDe(posicion)
                                       - edificio->pisoDe(posicionRuido));
        alcance *= std::pow(0.5, diferenciaPisos);
    }

    if (distancia.length() > alcance || intensidad <= 0.0) {
        return false;
    }

    bool nuevoObjetivo = !hayRuidoPendiente
                         || QLineF(ultimaPosicionRuido, posicionRuido).length() > 10.0;
    ultimaPosicionRuido = posicionRuido;
    hayRuidoPendiente = true;
    if (nuevoObjetivo) {
        limpiarRutaConservandoEscalera();
        usarRutaAlternativa = QRandomGenerator::global()->generateDouble() >= probabilidadRutaOptima;
    }

    return true;
}

void GuardiaCentinela::ver(const QPointF& posicionMinion, bool usandoEscalera,
                           const QPointF& destinoEscalera) {
    viendoMinion = false;

    if (!edificio) {
        return;
    }

    const bool mismoPiso = edificio->pisoDe(posicion) == edificio->pisoDe(posicionMinion);

    if (mismoPiso && QLineF(posicion, posicionMinion).length() <= radioVision) {
        viendoMinion = true;
        ultimaPosicionVista = usandoEscalera ? destinoEscalera : posicionMinion;
        tiempoDesdeUltimaVista = 0.0;
    }
}

void GuardiaCentinela::decidir() {
    if (viendoMinion) {
        if (estadoActual != "Perseguir") {
            usarRutaAlternativa = QRandomGenerator::global()->generateDouble() >= probabilidadRutaOptima;
            limpiarRutaConservandoEscalera();
        }
        estadoActual = "Perseguir";
        return;
    }

    if (estadoActual == "Perseguir") {          // lo perdió de vista: va a donde lo vio
        if (tiempoDesdeUltimaVista < 2.5) {
            return;
        }
        ultimaPosicionRuido = ultimaPosicionVista;
        hayRuidoPendiente = true;
        limpiarRutaConservandoEscalera();
    }

    if (hayRuidoPendiente) {
        estadoActual = "Investigar";
    }
}

void GuardiaCentinela::actuar(double deltaTiempo) {
    if (!movimientoActivado) {
        return;
    }

    if (estadoActual == "Perseguir" && !viendoMinion) {
        tiempoDesdeUltimaVista += deltaTiempo;
    }

    // Termina el tramo vertical antes de recalcular por una alerta o persecución.
    if (estaEnEscalera()) {
        avanzarPorRuta(deltaTiempo, velocidad);
        return;
    }

    if (estadoActual == "Perseguir") {
        patrullaInicialPendiente = false;
        // En una escalera se conserva la ruta hasta terminar el trayecto.
        if (ruta.isEmpty() || !edificio
            || (edificio->pisoDe(posicion) == edificio->pisoDe(ultimaPosicionVista)
                && std::abs(posicion.y() - edificio->alturaDelPiso(edificio->pisoDe(posicion))) < 1.0)) {
            planificar(ultimaPosicionVista);
        }
        avanzarPorRuta(deltaTiempo, velocidad * 1.4);
        return;
    }

    if (estadoActual == "Investigar") {
        patrullaInicialPendiente = false;
        if (ruta.isEmpty()) {
            planificar(ultimaPosicionRuido);
        }

        avanzarPorRuta(deltaTiempo, velocidad);

        if (ruta.isEmpty()) {                       // llegó a donde sonó
            hayRuidoPendiente = false;
            estadoActual = alarmaActiva ? "Alerta" : "Patrullar";
            espera = alarmaActiva ? 0.0 : 1.0;
        }
        return;
    }

    // Patrullar
    if (espera > 0.0) {
        espera -= deltaTiempo;
        return;
    }

    if (ruta.isEmpty()) {
        QString zona;
        if (patrullaInicialPendiente && posicionesZonas.contains("Piso 3")) {
            zona = "Piso 3";
            patrullaInicialPendiente = false;
        } else {
            zona = elegirZonaDePatrulla();
        }
        if (zona.isEmpty()) {
            return;
        }
        usarRutaAlternativa = QRandomGenerator::global()->generateDouble() >= probabilidadRutaOptima;
        planificar(posicionesZonas.value(zona));
    }

    avanzarPorRuta(deltaTiempo, velocidad * (alarmaActiva ? 1.0 : 0.65));

    if (ruta.isEmpty()) {
        espera = alarmaActiva ? 0.0 : 1.5;
    }
}

void GuardiaCentinela::planificar(const QPointF& destino) {
    if (edificio) {
        ruta = edificio->planificarRuta(posicion, destino, usarRutaAlternativa);
    } else {
        ruta = QVector<QPointF>{destino};
    }
}

bool GuardiaCentinela::estaEnEscalera() const {
    if (!edificio || ruta.isEmpty()) return false;
    double alturaPiso = edificio->alturaDelPiso(edificio->pisoDe(posicion));
    return std::abs(posicion.y() - alturaPiso) > 0.01
           && std::abs(posicion.x() - ruta.first().x()) < 0.01;
}

void GuardiaCentinela::limpiarRutaConservandoEscalera() {
    if (estaEnEscalera()) {
        QPointF finEscalera = ruta.first();
        ruta.clear();
        ruta.append(finEscalera);
    } else {
        ruta.clear();
    }
}

void GuardiaCentinela::avanzarPorRuta(double deltaTiempo, double velocidadActual) {
    double restante = velocidadActual * deltaTiempo;

    while (!ruta.isEmpty() && restante > 0.0) {
        const QPointF objetivo = ruta.first();
        QLineF tramo(posicion, objetivo);

        if (tramo.length() <= restante) {
            posicion = objetivo;
            restante -= tramo.length();
            ruta.removeFirst();
        } else {
            tramo.setLength(restante);
            posicion = tramo.p2();
            restante = 0.0;
        }
    }
}

QString GuardiaCentinela::elegirZonaDePatrulla() const {
    if (posicionesZonas.isEmpty()) {
        return QString();
    }

    // Cada zona pesa 1 + 4*sospecha: patrulla más donde suele encontrar a Kevin
    double total = 0.0;
    for (auto it = posicionesZonas.constBegin(); it != posicionesZonas.constEnd(); ++it) {
        total += 1.0 + 4.0 * consultarZona(it.key());
    }

    double azar = QRandomGenerator::global()->generateDouble() * total;
    for (auto it = posicionesZonas.constBegin(); it != posicionesZonas.constEnd(); ++it) {
        azar -= 1.0 + 4.0 * consultarZona(it.key());
        if (azar <= 0.0) {
            return it.key();
        }
    }

    return posicionesZonas.constBegin().key();
}

void GuardiaCentinela::registrarZona(const QString& zona,
                                     const QPointF& posicionZona) {
    posicionesZonas[zona] = posicionZona;

    if (!mapaCalor.contains(zona)) {
        mapaCalor[zona] = 0.0;
    }
}

void GuardiaCentinela::aprender(const QString& zona, bool encontroMinion) {
    double recompensa = encontroMinion ? 1.0 : 0.0;
    double valorActual = mapaCalor.value(zona, 0.0);

    mapaCalor[zona] = valorActual + tasaAprendizaje * (recompensa - valorActual);
    if (encontroMinion) {
        probabilidadRutaOptima = std::min(0.90, probabilidadRutaOptima + 0.02);
    }
}

void GuardiaCentinela::reiniciar(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
    estadoActual = alarmaActiva ? "Alerta" : "Patrullar";
    ruta.clear();
    hayRuidoPendiente = false;
    viendoMinion = false;
    tiempoDesdeUltimaVista = 0.0;
    movimientoActivado = true;
    patrullaInicialPendiente = false;
    espera = 2.0;                       // margen para que Kevin no sea atrapado dos veces seguidas
}

QString GuardiaCentinela::obtenerEstadoActual() const {
    return estadoActual;
}

QPointF GuardiaCentinela::obtenerPosicion() const {
    return posicion;
}

double GuardiaCentinela::consultarZona(const QString& zona) const {
    return mapaCalor.value(zona, 0.0);
}

QString GuardiaCentinela::obtenerZonaMasSospechosa() const {
    QString zonaMasSospechosa;
    double valorMasAlto = 0.0;

    for (auto iterador = mapaCalor.constBegin();
         iterador != mapaCalor.constEnd();
         ++iterador) {
        if (iterador.value() > valorMasAlto) {
            valorMasAlto = iterador.value();
            zonaMasSospechosa = iterador.key();
        }
    }

    return zonaMasSospechosa;
}
