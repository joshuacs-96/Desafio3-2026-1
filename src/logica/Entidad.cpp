#include "Entidad.h"

Entidad::Entidad(const QPointF& posicionInicial, const QSizeF& tamano)
    : posicion(posicionInicial),
      velocidad(0.0, 0.0),
      hitbox(posicionInicial, tamano) {
}

bool Entidad::colisionaCon(const Entidad& otra) const {
    return hitbox.intersects(otra.hitbox);
}

QPointF Entidad::obtenerPosicion() const {
    return posicion;
}

QPointF Entidad::obtenerVelocidad() const {
    return velocidad;
}

QRectF Entidad::obtenerHitbox() const {
    return hitbox;
}

void Entidad::establecerPosicion(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
    hitbox.moveTopLeft(nuevaPosicion);
}

void Entidad::establecerVelocidad(const QPointF& nuevaVelocidad) {
    velocidad = nuevaVelocidad;
}
