#include "Nivel1Banco.h"

#include <QLineF>

Nivel1Banco::Nivel1Banco()
    : boveda(100.0, 3),
      camara(QPointF(260.0, 90.0), 180.0, 45.0, 1.5, 180.0),
      guardia(QPointF(80.0, 280.0), 260.0),
      botinRecolectado(0),
      radioCaptura(28.0) {
    iniciar();
}

void Nivel1Banco::iniciar() {
    completado = false;
    perdido = false;
    botinRecolectado = 0;

    minion.establecerPosicion(QPointF(40.0, 280.0));

    guardia.registrarZona("Entrada", QPointF(40.0, 280.0));
    guardia.registrarZona("Puerta", QPointF(180.0, 180.0));
    guardia.registrarZona("Boveda", QPointF(340.0, 100.0));
}

void Nivel1Banco::actualizar(double deltaTiempo) {
    if (completado || perdido) {
        return;
    }

    minion.actualizar(deltaTiempo);
    camara.actualizar(deltaTiempo);

    if (minion.hayRisaPendiente()) {
        procesarRuido(minion.generarRisaAutomatica());
    }

    if (camara.detecta(minion)) {
        Ruido alarma {"Alarma de camara", minion.obtenerPosicion(), 10.0, 1.0};
        procesarRuido(alarma);
    }

    guardia.decidir();

    QString estadoAntesDeActuar = guardia.obtenerEstadoActual();
    guardia.actuar(deltaTiempo);

    QLineF distancia(guardia.obtenerPosicion(), minion.obtenerPosicion());

    if (estadoAntesDeActuar == "Investigar"
        && guardia.obtenerEstadoActual() == "Patrullar"
        && distancia.length() > radioCaptura) {
        guardia.aprender(determinarZona(guardia.obtenerPosicion()), false);
    }

    verificarCaptura();
    verificarObjetivo();
}

void Nivel1Banco::moverMinion(const QPointF& direccion, double deltaTiempo) {
    if (!completado && !perdido) {
        minion.mover(direccion, deltaTiempo);
    }
}

void Nivel1Banco::danarPuerta(double dano) {
    if (completado || perdido || dano <= 0.0) {
        return;
    }

    boveda.recibirDanio(dano);

    Ruido golpe {"Golpe a la puerta", minion.obtenerPosicion(), 7.0, 1.0};
    procesarRuido(golpe);
}

void Nivel1Banco::recogerBotin(int cantidad) {
    if (!boveda.estaAbierta() || cantidad <= 0) {
        return;
    }

    botinRecolectado += cantidad;
    verificarObjetivo();
}

void Nivel1Banco::usarBanana() {
    minion.comerBanana();
}

const Minion& Nivel1Banco::obtenerMinion() const {
    return minion;
}

const Boveda& Nivel1Banco::obtenerBoveda() const {
    return boveda;
}

const GuardiaCentinela& Nivel1Banco::obtenerGuardia() const {
    return guardia;
}

int Nivel1Banco::obtenerBotinRecolectado() const {
    return botinRecolectado;
}

QString Nivel1Banco::obtenerZonaActual() const {
    return determinarZona(minion.obtenerPosicion());
}

void Nivel1Banco::procesarRuido(const Ruido& ruido) {
    guardia.percibir(ruido.posicion, ruido.intensidad);
}

void Nivel1Banco::verificarCaptura() {
    QLineF distancia(guardia.obtenerPosicion(), minion.obtenerPosicion());

    if (distancia.length() > radioCaptura) {
        return;
    }

    QString zonaCaptura = determinarZona(minion.obtenerPosicion());

    minion.recibirDanio(100);
    guardia.aprender(zonaCaptura, true);
    minion.establecerPosicion(QPointF(40.0, 280.0));

    if (minion.obtenerVidas() == 0) {
        perdido = true;
    }
}

void Nivel1Banco::verificarObjetivo() {
    if (boveda.estaAbierta()
        && botinRecolectado >= boveda.obtenerBotinMinimo()) {
        completado = true;
    }
}

QString Nivel1Banco::determinarZona(const QPointF& posicion) const {
    if (posicion.x() < 120.0) {
        return "Entrada";
    }

    if (posicion.x() < 270.0) {
        return "Puerta";
    }

    return "Boveda";
}
