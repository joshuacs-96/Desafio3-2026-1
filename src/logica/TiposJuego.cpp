#include "TiposJuego.h"

bool Zona::operator<(const Zona& otra) const {
    if (fila != otra.fila) {
        return fila < otra.fila;
    }

    return columna < otra.columna;
}
