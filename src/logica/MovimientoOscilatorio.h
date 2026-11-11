#pragma once

#include "Fisica.h"

class MovimientoOscilatorio : public Fisica {
public:
    MovimientoOscilatorio(double amplitud, double frecuenciaAngular, double fase);

    QPointF calcular(double tiempo) const override;

private:
    double amplitud;
    double frecuenciaAngular;
    double fase;
};
