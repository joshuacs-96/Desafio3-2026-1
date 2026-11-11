#pragma once

#include "Fisica.h"

class MovimientoParabolico : public Fisica {
public:
    MovimientoParabolico(double velocidadInicial,
                         double anguloEnRadianes,
                         double gravedad);

    QPointF calcular(double tiempo) const override;

private:
    double velocidadInicial;
    double anguloEnRadianes;
    double gravedad;
};
