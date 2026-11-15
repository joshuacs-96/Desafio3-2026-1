#pragma once

#include <QPointF>

#include "Ruido.h"

class Minion {
public:
    Minion();

    void mover(const QPointF& direccion, double deltaTiempo);
    void saltar();
    void recibirDanio(int dano);
    void comerBanana();
    void actualizar(double deltaTiempo);

    bool hayRisaPendiente() const;
    Ruido generarRisaAutomatica();

    QPointF obtenerPosicion() const;
    int obtenerEnergia() const;
    int obtenerVidas() const;
    bool tieneBananaActiva() const;

private:
    QPointF posicion;
    QPointF velocidad;
    int energia;
    int vidas;
    bool modoBanana;
    bool risaPendiente;
    double duracionBananaRestante;
    double tiempoHastaSiguienteRisa;
    double velocidadBase;
};
