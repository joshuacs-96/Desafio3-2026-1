#include "CarroPolicia.h"

#include <QLineF>

#include "CarroJugador.h"

CarroPolicia::CarroPolicia(const QPointF& posicionInicial,
                           double radioSensor,
                           double tiempoMaximoCerca)
    : posicion(posicionInicial),
    radioSensor(radioSensor),
    tiempoMaximoCerca(tiempoMaximoCerca),
    tiempoCerca(0.0),
    cercaDelJugador(false) {
}

void CarroPolicia::actualizar(const CarroJugador& carroJugador,
                              double deltaTiempo) {
    QLineF distancia(posicion, carroJugador.obtenerPosicion());
    cercaDelJugador = distancia.length() <= radioSensor;

    if (cercaDelJugador) {
        tiempoCerca += deltaTiempo;
    } else {
        tiempoCerca = 0.0;
    }
}

void CarroPolicia::moverVerticalmente(double desplazamiento) {
    posicion.ry() += desplazamiento;
}

void CarroPolicia::moverHorizontalmente(double desplazamiento) {
    posicion.rx() += desplazamiento;
}

void CarroPolicia::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
}

void CarroPolicia::reiniciarTiempoCerca() {
    tiempoCerca = 0.0;
}

void CarroPolicia::configurarSensor(double nuevoRadio,
                                    double nuevoTiempoMaximo) {
    radioSensor = nuevoRadio;
    tiempoMaximoCerca = nuevoTiempoMaximo;
    reiniciarTiempoCerca();
}

QPointF CarroPolicia::obtenerPosicion() const {
    return posicion;
}

double CarroPolicia::obtenerRadioSensor() const {
    return radioSensor;
}

double CarroPolicia::obtenerTiempoCerca() const {
    return tiempoCerca;
}

bool CarroPolicia::estaCercaDelJugador() const {
    return cercaDelJugador;
}

bool CarroPolicia::superoTiempoCerca() const {
    return tiempoCerca >= tiempoMaximoCerca;
}
