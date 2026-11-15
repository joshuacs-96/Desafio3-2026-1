#include "CamaraSeguridad.h"

#include <QLineF>

#include <cmath>

#include "Minion.h"

namespace {
double normalizarDiferenciaAngular(double diferencia) {
    while (diferencia > 180.0) {
        diferencia -= 360.0;
    }

    while (diferencia < -180.0) {
        diferencia += 360.0;
    }

    return std::abs(diferencia);
}
}

CamaraSeguridad::CamaraSeguridad(const QPointF& posicion,
                                 double anguloCentro,
                                 double amplitud,
                                 double frecuenciaAngular,
                                 double alcance)
    : posicion(posicion),
    anguloCentro(anguloCentro),
    amplitud(amplitud),
    frecuenciaAngular(frecuenciaAngular),
    alcance(alcance),
    anguloActual(anguloCentro),
    tiempoTranscurrido(0.0),
    aperturaCono(60.0) {
}

void CamaraSeguridad::actualizar(double deltaTiempo) {
    tiempoTranscurrido += deltaTiempo;
    anguloActual = anguloCentro + amplitud * std::sin(frecuenciaAngular * tiempoTranscurrido);
}

bool CamaraSeguridad::detecta(const Minion& minion) const {
    QLineF lineaVision(posicion, minion.obtenerPosicion());

    if (lineaVision.length() > alcance) {
        return false;
    }

    double diferencia = normalizarDiferenciaAngular(lineaVision.angle() - anguloActual);

    return diferencia <= aperturaCono / 2.0;
}

double CamaraSeguridad::obtenerAnguloActual() const {
    return anguloActual;
}
