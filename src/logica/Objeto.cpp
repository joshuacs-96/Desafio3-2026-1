#include "Objeto.h"

Objeto::Objeto(TipoObjeto tipo,
               const QPointF& posicionInicial,
               int valor,
               int dano)
    : Entidad(posicionInicial, QSizeF(32.0, 32.0)),
      tipo(tipo),
      valor(valor),
      dano(dano) {
}

void Objeto::actualizar(double deltaTiempo) {
    (void)deltaTiempo;
}

TipoObjeto Objeto::obtenerTipo() const {
    return tipo;
}

int Objeto::obtenerValor() const {
    return valor;
}

int Objeto::obtenerDano() const {
    return dano;
}
