#include "Personaje.h"

#include <algorithm>

Personaje::Personaje(const QPointF& posicionInicial)
    : Entidad(posicionInicial, QSizeF(32.0, 48.0)),
      energia(100),
      vidas(3),
      modoFortachon(false),
      tiempoFortachonRestante(0.0),
      tiempoEnfriamientoRisa(0.0),
      velocidadBase(150.0) {
}

void Personaje::actualizar(double deltaTiempo) {
    if (tiempoFortachonRestante > 0.0) {
        tiempoFortachonRestante -= deltaTiempo;
        modoFortachon = tiempoFortachonRestante > 0.0;
    }

    tiempoEnfriamientoRisa = std::max(0.0, tiempoEnfriamientoRisa - deltaTiempo);
}

void Personaje::mover(const QPointF& direccion, double deltaTiempo) {
    double multiplicadorVelocidad = modoFortachon ? 0.75 : 1.0;
    QPointF desplazamiento = direccion * velocidadBase * multiplicadorVelocidad * deltaTiempo;

    establecerPosicion(posicion + desplazamiento);
}

double Personaje::obtenerDanioPuerta(double deltaTiempo) const {
    double danioPorSegundo = modoFortachon ? 40.0 : 20.0;

    return danioPorSegundo * deltaTiempo;
}

void Personaje::comerBanana() {
    modoFortachon = true;
    tiempoFortachonRestante = 10.0;
}

Ruido Personaje::usarDistraccion() {
    if (tiempoEnfriamientoRisa > 0.0) {
        return generarRuido(TipoRuido::Risa, 0.0, 0.0);
    }

    tiempoEnfriamientoRisa = 6.0;

    return generarRuido(TipoRuido::Risa, 6.0, 3.0);
}

Ruido Personaje::generarRuido(TipoRuido tipo,
                               double intensidad,
                               double duracion) const {
    double multiplicadorRuido = modoFortachon ? 1.5 : 1.0;

    return Ruido {tipo, posicion, intensidad * multiplicadorRuido, duracion};
}

int Personaje::obtenerEnergia() const {
    return energia;
}

int Personaje::obtenerVidas() const {
    return vidas;
}

bool Personaje::estaFortachon() const {
    return modoFortachon;
}
