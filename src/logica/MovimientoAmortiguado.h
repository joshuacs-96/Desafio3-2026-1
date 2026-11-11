#pragma once

#include "Fisica.h"

class MovimientoAmortiguado : public Fisica {
public:
    MovimientoAmortiguado(double velocidadInicial, double coeficienteFriccion);

    QPointF calcular(double tiempo) const override;

private:
    double velocidadInicial;
    double coeficienteFriccion;
};
