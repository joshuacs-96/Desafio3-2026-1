#pragma once

#include <QPointF>

class CarroJugador {
public:
    CarroJugador();

    void acelerar(double cantidad);
    void frenar(double cantidad);
    void girar(double direccion);
    void recibirDanio(int dano);
    void perderBotin(int cantidad);
    void activarDerrape(double duracion);
    void rebotar();
    void actualizar(double deltaTiempo);

    QPointF obtenerPosicion() const;
    double obtenerIntegridad() const;
    int obtenerBotinActual() const;
    bool estaDerrapando() const;

private:
    QPointF posicion;
    QPointF velocidad;
    double integridad;
    int botinActual;
    bool derrapando;
    double muAceite;
    double coeficienteRebote;
    double tiempoDerrapeRestante;
};
