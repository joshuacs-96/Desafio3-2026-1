#pragma once

#include "Entidad.h"
#include "TiposJuego.h"

class Personaje : public Entidad {
public:
    Personaje(const QPointF& posicionInicial);

    void actualizar(double deltaTiempo) override;
    void mover(const QPointF& direccion, double deltaTiempo);

    double obtenerDanioPuerta(double deltaTiempo) const;
    void comerBanana();
    Ruido usarDistraccion();
    Ruido generarRuido(TipoRuido tipo, double intensidad, double duracion) const;

    int obtenerEnergia() const;
    int obtenerVidas() const;
    bool estaFortachon() const;

private:
    int energia;
    int vidas;
    bool modoFortachon;
    double tiempoFortachonRestante;
    double tiempoEnfriamientoRisa;
    double velocidadBase;
};
