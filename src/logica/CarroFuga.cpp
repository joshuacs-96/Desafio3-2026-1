#include "CarroFuga.h"

#include <algorithm>

CarroFuga::CarroFuga(const QPointF& posicionInicial, int botinInicial)
    : Entidad(posicionInicial, QSizeF(48.0, 80.0)),
      integridad(100.0),
      botinActual(botinInicial),
      derrapando(false),
      tiempoDerrapeRestante(0.0),
      velocidadMaxima(260.0) {
}

void CarroFuga::actualizar(double deltaTiempo) {
    establecerPosicion(posicion + velocidad * deltaTiempo);

    if (tiempoDerrapeRestante > 0.0) {
        tiempoDerrapeRestante -= deltaTiempo;
        derrapando = tiempoDerrapeRestante > 0.0;
    }
}

void CarroFuga::acelerar(double cantidad) {
    velocidad.setY(std::max(-velocidadMaxima, velocidad.y() - cantidad));
}

void CarroFuga::frenar(double cantidad) {
    velocidad.setY(std::min(0.0, velocidad.y() + cantidad));
}

void CarroFuga::girar(double direccion) {
    double multiplicadorGiro = derrapando ? 0.35 : 1.0;

    velocidad.setX(direccion * 180.0 * multiplicadorGiro);
}

void CarroFuga::recibirImpacto(int dano, int botinPerdido) {
    integridad = std::max(0.0, integridad - dano);
    botinActual = std::max(0, botinActual - botinPerdido);
}

void CarroFuga::activarDerrape(double duracion) {
    derrapando = true;
    tiempoDerrapeRestante = duracion;
}

double CarroFuga::obtenerIntegridad() const {
    return integridad;
}

int CarroFuga::obtenerBotinActual() const {
    return botinActual;
}

bool CarroFuga::estaDerrapando() const {
    return derrapando;
}
