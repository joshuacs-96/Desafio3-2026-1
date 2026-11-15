#include "Escalera.h"

#include "Minion.h"

Escalera::Escalera(const QPointF& posicion, const QPointF& destino)
    : posicion(posicion),
    destino(destino) {
}

void Escalera::usar(Minion& minion) const {
    minion.establecerPosicion(destino);
}

QPointF Escalera::obtenerPosicion() const {
    return posicion;
}

QPointF Escalera::obtenerDestino() const {
    return destino;
}