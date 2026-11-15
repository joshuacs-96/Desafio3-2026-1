#pragma once

#include <QPointF>

class Minion;

class CamaraSeguridad {
public:
    CamaraSeguridad(const QPointF& posicion,
                    double anguloCentro,
                    double amplitud,
                    double frecuenciaAngular,
                    double alcance);

    void actualizar(double deltaTiempo);
    bool detecta(const Minion& minion) const;

    double obtenerAnguloActual() const;

private:
    QPointF posicion;
    double anguloCentro;
    double amplitud;
    double frecuenciaAngular;
    double alcance;
    double anguloActual;
    double tiempoTranscurrido;
    double aperturaCono;
};
