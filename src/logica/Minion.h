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
    QPointF obtenerPosicionSuelo() const;     // posición sin contar la altura del salto
    void establecerPosicion(const QPointF& nuevaPosicion);
    int obtenerEnergia() const;
    int obtenerVidas() const;
    bool tieneBananaActiva() const;
    bool estaSaltando() const;
    void iniciarEscalera(const QPointF& destino);
    bool estaEnEscalera() const;
    QPointF obtenerDestinoEscalera() const;
    void limitarHorizontal(double minimo, double maximo);
    double obtenerVelocidadVertical() const;  // > 0 subiendo, < 0 cayendo

private:
    void actualizarSalto(double deltaTiempo);
    void actualizarEscalera(double deltaTiempo);

    QPointF posicion;
    QPointF velocidad;
    int energia;
    int vidas;
    bool modoBanana;
    bool risaPendiente;
    double duracionBananaRestante;
    double tiempoHastaSiguienteRisa;
    double velocidadBase;
    bool enSalto;
    double tiempoSalto;
    double ySuelo;
    double velocidadSaltoInicial;
    double gravedad;
    bool escalando;
    QPointF destinoEscalera;
    double velocidadEscalera;
};
