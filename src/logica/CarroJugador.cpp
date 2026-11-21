#include "CarroJugador.h"

#include <algorithm>
#include <cmath>

CarroJugador::CarroJugador()
    : posicion(0.0, 0.0),
      velocidad(0.0, 0.0),
      integridad(100.0),
      botinActual(0),
      derrapando(false),
      muAceite(0.45),
      coeficienteRebote(0.5),
      tiempoDerrapeRestante(0.0) {
}

void CarroJugador::acelerar(double cantidad) {
    velocidad.setY(velocidad.y() - cantidad);
}

void CarroJugador::frenar(double cantidad) {
    velocidad.setY(std::min(0.0, velocidad.y() + cantidad));
}

void CarroJugador::girar(double direccion) {
    double controlDireccion = derrapando ? 0.35 : 1.0;
    velocidad.setX(direccion * 180.0 * controlDireccion);
}

void CarroJugador::recibirDanio(int dano) {
    integridad = std::max(0.0, integridad - dano);
}

void CarroJugador::perderBotin(int cantidad) {
    botinActual = std::max(0, botinActual - cantidad);
}

void CarroJugador::activarDerrape(double duracion) {
    derrapando = true;
    tiempoDerrapeRestante = duracion;
}

void CarroJugador::rebotar() {
    velocidad *= -coeficienteRebote;
}

void CarroJugador::detener() {
    velocidad = QPointF(0.0, 0.0);
    derrapando = false;
    tiempoDerrapeRestante = 0.0;
}

void CarroJugador::actualizar(double deltaTiempo) {
    if (derrapando) {
        double factorFriccion = std::exp(-muAceite * deltaTiempo);
        velocidad *= factorFriccion;
        tiempoDerrapeRestante -= deltaTiempo;

        if (tiempoDerrapeRestante <= 0.0) {
            derrapando = false;
        }
    }
    velocidad.setY(velocidad.y() * std::exp(-1.5 * deltaTiempo));
    velocidad.setY(std::clamp(velocidad.y(), -220.0, 220.0));

    posicion += velocidad * deltaTiempo;
}

void CarroJugador::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
}

void CarroJugador::establecerIntegridad(double nuevaIntegridad) {
    integridad = std::max(0.0, nuevaIntegridad);
}

void CarroJugador::establecerBotinActual(int nuevoBotin) {
    botinActual = std::max(0, nuevoBotin);
}

QPointF CarroJugador::obtenerPosicion() const {
    return posicion;
}

double CarroJugador::obtenerIntegridad() const {
    return integridad;
}

int CarroJugador::obtenerBotinActual() const {
    return botinActual;
}

bool CarroJugador::estaDerrapando() const {
    return derrapando;
}

bool CarroJugador::estaDestruido() const {
    return integridad <= 0.0;
}
