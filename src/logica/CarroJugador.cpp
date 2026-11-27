// CarroJugador.cpp
#include "CarroJugador.h"

#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

namespace {
constexpr double VELOCIDAD_CAMBIO_CARRIL = 190.0;   // unidades lógicas por segundo
constexpr double CONTROL_EN_DERRAPE = 0.35;         // sobre el aceite casi no obedece
constexpr double IMPULSO_DERRAPE = 120.0;           // empujón lateral al pisar el aceite
}

CarroJugador::CarroJugador()
    : posicion(0.0, 0.0),
    velocidad(0.0, 0.0),
    integridad(100.0),
    botinActual(0),
    derrapando(false),
    muAceite(1.2),
    coeficienteRebote(0.5),
    tiempoDerrapeRestante(0.0),
    protegido(false),
    tiempoEscudoRestante(0.0){
}

void CarroJugador::cambiarCarril(double direccion) {
    const double control = derrapando ? CONTROL_EN_DERRAPE : 1.0;
    velocidad.setY(direccion * VELOCIDAD_CAMBIO_CARRIL * control);
}

void CarroJugador::recibirDanio(int dano) {
    integridad = std::max(0.0, integridad - dano);
}

void CarroJugador::perderBotin(int cantidad) {
    botinActual = std::max(0, botinActual - cantidad);
}

void CarroJugador::activarDerrape(double duracion) {
    if (!derrapando && std::abs(velocidad.y()) < 40.0) {
        // al pisar el aceite el carro se desliza hacia un lado
        const double signo = QRandomGenerator::global()->bounded(2) == 0 ? -1.0 : 1.0;
        velocidad.setY(signo * IMPULSO_DERRAPE);
    }
    derrapando = true;
    tiempoDerrapeRestante = duracion;
}
void CarroJugador::recuperarIntegridad(double cantidad) {
    integridad = std::min(100.0, integridad + cantidad);
}

void CarroJugador::activarEscudo(double duracion) {
    protegido = true;
    tiempoEscudoRestante = duracion;
}

bool CarroJugador::estaProtegido() const {
    return protegido;
}

double CarroJugador::obtenerTiempoEscudo() const {
    return tiempoEscudoRestante;
}

void CarroJugador::rebotar() {
    velocidad *= -coeficienteRebote;
}

void CarroJugador::detener() {
    velocidad = QPointF(0.0, 0.0);
    derrapando = false;
    tiempoDerrapeRestante = 0.0;
    protegido = false;
    tiempoEscudoRestante = 0.0;
}

void CarroJugador::actualizar(double deltaTiempo) {
    if (protegido) {
        tiempoEscudoRestante -= deltaTiempo;

        if (tiempoEscudoRestante <= 0.0) {
            tiempoEscudoRestante = 0.0;
            protegido = false;
        }
    }
    if (derrapando) {
        // Fricción con desaceleración exponencial: v(t) = v0 * e^(-mu*t)
        velocidad *= std::exp(-muAceite * deltaTiempo);
        tiempoDerrapeRestante -= deltaTiempo;

        if (tiempoDerrapeRestante <= 0.0) {
            derrapando = false;
        }
    }

    posicion += velocidad * deltaTiempo;
}

void CarroJugador::establecerPosicion(const QPointF& nuevaPosicion) { posicion = nuevaPosicion; }
void CarroJugador::establecerIntegridad(double nuevaIntegridad) { integridad = std::max(0.0, nuevaIntegridad); }
void CarroJugador::establecerBotinActual(int nuevoBotin) { botinActual = std::max(0, nuevoBotin); }

QPointF CarroJugador::obtenerPosicion() const { return posicion; }
double CarroJugador::obtenerIntegridad() const { return integridad; }
int CarroJugador::obtenerBotinActual() const { return botinActual; }
bool CarroJugador::estaDerrapando() const { return derrapando; }
bool CarroJugador::estaDestruido() const { return integridad <= 0.0; }