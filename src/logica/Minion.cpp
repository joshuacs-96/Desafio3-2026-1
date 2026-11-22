#include "Minion.h"
#include <QLineF>
#include <algorithm>

Minion::Minion()
    : posicion(0.0, 0.0),
    velocidad(0.0, 0.0),
    energia(100),
    vidas(2),
    modoBanana(false),
    risaPendiente(false),
    duracionBananaRestante(0.0),
    tiempoHastaSiguienteRisa(0.0),
    velocidadBase(130.0),
    enSalto(false),
    tiempoSalto(0.0),
    ySuelo(0.0),
    velocidadSaltoInicial(320.0),
    gravedad(900.0),
    escalando(false),
    destinoEscalera(0.0, 0.0),
    velocidadEscalera(100.0){
}

void Minion::mover(const QPointF& direccion, double deltaTiempo) {
    if (escalando) {
        return;
    }

    double multiplicadorVelocidad = modoBanana ? 1.4 : 1.0;
    velocidad = direccion * velocidadBase * multiplicadorVelocidad;
    posicion += velocidad * deltaTiempo;
}

void Minion::saltar() {
    if (enSalto || escalando) {
        return;
    }

    enSalto = true;
    tiempoSalto = 0.0;
    ySuelo = posicion.y();
}

void Minion::actualizarSalto(double deltaTiempo) {
    if (!enSalto) {
        return;
    }

    tiempoSalto += deltaTiempo;

    // Movimiento parabólico: altura(t) = v0*t - 1/2*g*t^2
    const double altura = velocidadSaltoInicial * tiempoSalto
                          - 0.5 * gravedad * tiempoSalto * tiempoSalto;

    if (altura <= 0.0) {        // volvió al suelo
        enSalto = false;
        posicion.setY(ySuelo);
        return;
    }

    posicion.setY(ySuelo - altura);   // en pantalla la y crece hacia abajo
}

void Minion::recibirDanio(int dano) {
    energia = std::max(0, energia - dano);

    if (energia == 0 && vidas > 0) {
        vidas--;
        energia = 100;
    }
}

void Minion::comerBanana() {
    modoBanana = true;
    duracionBananaRestante = 8.0;
    tiempoHastaSiguienteRisa = 3.0;
}

void Minion::actualizar(double deltaTiempo) {
    actualizarEscalera(deltaTiempo);

    if (!escalando) {
        actualizarSalto(deltaTiempo);
    }

    if (!modoBanana) {
        return;
    }

    duracionBananaRestante -= deltaTiempo;
    tiempoHastaSiguienteRisa -= deltaTiempo;

    if (tiempoHastaSiguienteRisa <= 0.0) {
        risaPendiente = true;
        tiempoHastaSiguienteRisa = 3.0;
    }

    if (duracionBananaRestante <= 0.0) {
        modoBanana = false;
        risaPendiente = false;
    }
}

bool Minion::hayRisaPendiente() const {
    return risaPendiente;
}

Ruido Minion::generarRisaAutomatica() {
    risaPendiente = false;

    return Ruido {"Risa", posicion, 6.0, 1.0};
}

QPointF Minion::obtenerPosicion() const {
    return posicion;
}

QPointF Minion::obtenerPosicionSuelo() const {
    return enSalto ? QPointF(posicion.x(), ySuelo) : posicion;
}

void Minion::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
    enSalto = false;
    escalando = false;
}
int Minion::obtenerEnergia() const {
    return energia;
}

int Minion::obtenerVidas() const {
    return vidas;
}

bool Minion::tieneBananaActiva() const {
    return modoBanana;
}

bool Minion::estaSaltando() const {
    return enSalto;
}

void Minion::iniciarEscalera(const QPointF& destino) {
    if (escalando || enSalto) {
        return;
    }

    escalando = true;
    destinoEscalera = destino;
    velocidad = QPointF(0.0, 0.0);
}

bool Minion::estaEnEscalera() const {
    return escalando;
}

double Minion::obtenerVelocidadVertical() const {
    return enSalto ? velocidadSaltoInicial - gravedad * tiempoSalto : 0.0;
}

void Minion::limitarHorizontal(double minimo, double maximo) {
    posicion.setX(std::clamp(posicion.x(), minimo, maximo));   // no toca el salto
}

void Minion::actualizarEscalera(double deltaTiempo) {
    if (!escalando) {
        return;
    }

    QLineF recorrido(posicion, destinoEscalera);
    const double distancia = recorrido.length();
    const double avance = velocidadEscalera * deltaTiempo;

    if (distancia <= avance || distancia < 0.01) {
        posicion = destinoEscalera;
        escalando = false;
        return;
    }

    recorrido.setLength(avance);
    posicion = recorrido.p2();
}