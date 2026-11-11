#include "MovimientoOscilatorio.h"

#include <cmath>

MovimientoOscilatorio::MovimientoOscilatorio(double amplitud,
                                               double frecuenciaAngular,
                                               double fase)
    : amplitud(amplitud),
      frecuenciaAngular(frecuenciaAngular),
      fase(fase) {
}

QPointF MovimientoOscilatorio::calcular(double tiempo) const {
    double desplazamientoX = amplitud * std::sin(frecuenciaAngular * tiempo + fase);

    return QPointF(desplazamientoX, 0.0);
}
