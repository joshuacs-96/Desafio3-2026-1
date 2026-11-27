
#pragma once

#include <QPointF>

class CarroJugador;

class CarroPolicia {
public:
    CarroPolicia();

    void configurar(bool activo, double radioSensor, double tiempoMaximoCerca,
                    double velocidadX, double pausaEntrePasadas);
    void reiniciar();
    void actualizar(const CarroJugador& jugador, double deltaTiempo);
    void registrarGolpe();

    bool estaActivo() const;
    bool estaEnPantalla() const;
    bool estaCercaDelJugador() const;
    bool debeGolpear() const;
    QPointF obtenerPosicion() const;
    double obtenerRadioSensor() const;
    double obtenerTiempoCerca() const;
    void activarEmbestida();

private:
    void aparecer(const CarroJugador& jugador);

    bool embistiendo;
    QPointF posicion;
    bool activo;
    bool enPantalla;
    bool yaGolpeo;
    bool cercaDelJugador;
    bool retrocediendo;

    double radioSensor;
    double tiempoMaximoCerca;
    double tiempoCerca;
    double velocidadX;
    double velocidadLateral;
    double pausaEntrePasadas;
    double tiempoEspera;
};
