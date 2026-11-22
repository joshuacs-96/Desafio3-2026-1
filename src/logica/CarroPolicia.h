#pragma once

#include <QPointF>

class CarroJugador;

class CarroPolicia {
public:
    CarroPolicia(const QPointF& posicionInicial,
                 double radioSensor,
                 double tiempoMaximoCerca);

    void actualizar(const CarroJugador& carroJugador, double deltaTiempo);
    void moverVerticalmente(double desplazamiento);
    void moverHorizontalmente(double desplazamiento);
    void establecerPosicion(const QPointF& nuevaPosicion);
    void reiniciarTiempoCerca();
    void configurarSensor(double nuevoRadio, double nuevoTiempoMaximo);

    QPointF obtenerPosicion() const;
    double obtenerRadioSensor() const;
    double obtenerTiempoCerca() const;
    bool estaCercaDelJugador() const;
    bool superoTiempoCerca() const;

private:
    QPointF posicion;
    double radioSensor;
    double tiempoMaximoCerca;
    double tiempoCerca;
    bool cercaDelJugador;
};
