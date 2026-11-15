#include "Minion.h"

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
      velocidadBase(150.0) {
}

void Minion::mover(const QPointF& direccion, double deltaTiempo) {
    double multiplicadorVelocidad = modoBanana ? 1.4 : 1.0;
    velocidad = direccion * velocidadBase * multiplicadorVelocidad;
    posicion += velocidad * deltaTiempo;
}

void Minion::saltar() {
    // El movimiento parabólico del salto se implementará en Nivel1Banco.
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

void Minion::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
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
