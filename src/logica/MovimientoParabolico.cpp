#include "MovimientoParabolico.h"

#include <cmath>

MovimientoParabolico::MovimientoParabolico(double velocidadInicial,
                                             double anguloEnRadianes,
                                             double gravedad)
    : velocidadInicial(velocidadInicial),
      anguloEnRadianes(anguloEnRadianes),
      gravedad(gravedad) {
}

QPointF MovimientoParabolico::calcular(double tiempo) const {
    double posicionX = velocidadInicial * std::cos(anguloEnRadianes) * tiempo;
    double posicionY = velocidadInicial * std::sin(anguloEnRadianes) * tiempo
                       - 0.5 * gravedad * tiempo * tiempo;

    return QPointF(posicionX, posicionY);
}
