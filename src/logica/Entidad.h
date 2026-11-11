#pragma once

#include <QPointF>
#include <QRectF>

class Entidad {
public:
    Entidad(const QPointF& posicionInicial, const QSizeF& tamano);
    virtual ~Entidad() = default;

    virtual void actualizar(double deltaTiempo) = 0;

    bool colisionaCon(const Entidad& otra) const;

    QPointF obtenerPosicion() const;
    QPointF obtenerVelocidad() const;
    QRectF obtenerHitbox() const;

    void establecerPosicion(const QPointF& nuevaPosicion);
    void establecerVelocidad(const QPointF& nuevaVelocidad);

protected:
    QPointF posicion;
    QPointF velocidad;
    QRectF hitbox;
};
