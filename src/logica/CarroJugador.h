#pragma once

#include <QPointF>

class CarroJugador {
public:
    CarroJugador();

    void cambiarCarril(double direccion);    // -1 sube, +1 baja, 0 = suelta la tecla
    void recibirDanio(int dano);
    void perderBotin(int cantidad);
    void activarDerrape(double duracion);
    void recuperarIntegridad(double cantidad);
    void activarEscudo(double duracion);
    bool estaProtegido() const;
    double obtenerTiempoEscudo() const;
    void rebotar();
    void detener();
    void actualizar(double deltaTiempo);

    void establecerPosicion(const QPointF& nuevaPosicion);
    void establecerIntegridad(double nuevaIntegridad);
    void establecerBotinActual(int nuevoBotin);

    QPointF obtenerPosicion() const;
    double obtenerIntegridad() const;
    int obtenerBotinActual() const;
    bool estaDerrapando() const;
    bool estaDestruido() const;

private:
    QPointF posicion;
    QPointF velocidad;
    double integridad;
    int botinActual;
    bool derrapando;
    double muAceite;
    double coeficienteRebote;
    double tiempoDerrapeRestante;
    bool protegido;
    double tiempoEscudoRestante;
};