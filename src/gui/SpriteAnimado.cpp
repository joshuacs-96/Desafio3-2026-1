#include "SpriteAnimado.h"

#include <QTransform>

SpriteAnimado::SpriteAnimado(const QPixmap& tira, int cuadros, double fps,
                             const QSizeF& tamano, bool anclaPies)
    : tira(tira), cuadros(cuadros), fps(fps), tamano(tamano), anclaPies(anclaPies) {
    actualizarPixmap();
}

void SpriteAnimado::cambiarTira(const QPixmap& nuevaTira, int nuevosCuadros, double nuevosFps) {
    if (nuevaTira.cacheKey() == tira.cacheKey() && nuevosCuadros == cuadros) {
        fps = nuevosFps;            // misma animación: solo puede cambiar la velocidad
        return;
    }
    tira = nuevaTira;
    cuadros = nuevosCuadros;
    fps = nuevosFps;
    cuadroActual = 0;
    acumulado = 0.0;
    actualizarPixmap();
}

void SpriteAnimado::avanzar(double deltaTiempo) {
    if (cuadros <= 1 || fps <= 0.0) {
        return;
    }

    acumulado += deltaTiempo;
    const double paso = 1.0 / fps;
    bool cambio = false;

    while (acumulado >= paso) {
        acumulado -= paso;
        cuadroActual = (cuadroActual + 1) % cuadros;
        cambio = true;
    }

    if (cambio) {
        actualizarPixmap();
    }
}

void SpriteAnimado::fijarCuadro(int indice) {
    indice = qBound(0, indice, cuadros - 1);
    if (indice == cuadroActual) {
        return;
    }
    cuadroActual = indice;
    actualizarPixmap();
}

void SpriteAnimado::fijarTamano(const QSizeF& nuevoTamano) {
    if (nuevoTamano == tamano) {
        return;
    }
    tamano = nuevoTamano;
    actualizarPixmap();
}

void SpriteAnimado::espejar(bool alReves) {
    if (alReves == espejado) {
        return;
    }
    espejado = alReves;
    actualizarPixmap();
}

void SpriteAnimado::actualizarPixmap() {
    const int ancho = tira.width() / cuadros;
    QPixmap cuadro = tira.copy(cuadroActual * ancho, 0, ancho, tira.height());
    cuadro = cuadro.scaled(tamano.toSize(), Qt::KeepAspectRatio, Qt::FastTransformation);

    if (espejado) {
        cuadro = cuadro.transformed(QTransform().scale(-1, 1));
    }

    setPixmap(cuadro);
    setOffset(-cuadro.width() / 2.0,
              anclaPies ? -cuadro.height() : -cuadro.height() / 2.0);
}