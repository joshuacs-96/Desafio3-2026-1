#pragma once

#include <QPointF>

class Minion;

class Escalera {
public:
    Escalera(const QPointF& posicion, const QPointF& destino);

    void usar(Minion& minion) const;

    QPointF obtenerPosicion() const;
    QPointF obtenerDestino() const;

private:
    QPointF posicion;
    QPointF destino;
};
