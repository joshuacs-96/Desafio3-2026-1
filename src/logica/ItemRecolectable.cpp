#include "ItemRecolectable.h"

ItemRecolectable::ItemRecolectable(const QString& tipo, const QPointF& posicion,
                                   int valor, bool dentroDeBoveda)
    : tipo(tipo),
    posicion(posicion),
    valor(valor),
    dentroDeBoveda(dentroDeBoveda),
    recogido(false) {
}

QString ItemRecolectable::obtenerTipo() const { return tipo; }
QPointF ItemRecolectable::obtenerPosicion() const { return posicion; }
int ItemRecolectable::obtenerValor() const { return valor; }
bool ItemRecolectable::estaDentroDeBoveda() const { return dentroDeBoveda; }
bool ItemRecolectable::fueRecogido() const { return recogido; }
void ItemRecolectable::recoger() { recogido = true; }