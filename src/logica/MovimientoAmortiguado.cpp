#include "MovimientoAmortiguado.h"

#include <cmath>

MovimientoAmortiguado::MovimientoAmortiguado(double velocidadInicial,
                                               double coeficienteFriccion)
    : velocidadInicial(velocidadInicial),
      coeficienteFriccion(coeficienteFriccion) {
}

QPointF MovimientoAmortiguado::calcular(double tiempo) const {
    double velocidadActual = velocidadInicial * std::exp(-coeficienteFriccion * tiempo);

    return QPointF(velocidadActual, 0.0);
}
