#pragma once

#include "Entidad.h"

class CarroFuga : public Entidad {
public:
    CarroFuga(const QPointF& posicionInicial, int botinInicial);

    void actualizar(double deltaTiempo) override;
    void acelerar(double cantidad);
    void frenar(double cantidad);
    void girar(double direccion);
    void recibirImpacto(int dano, int botinPerdido);
    void activarDerrape(double duracion);

    double obtenerIntegridad() const;
    int obtenerBotinActual() const;
    bool estaDerrapando() const;

private:
    double integridad;
    int botinActual;
    bool derrapando;
    double tiempoDerrapeRestante;
    double velocidadMaxima;
};
