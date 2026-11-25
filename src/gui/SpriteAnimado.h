#pragma once
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QSizeF>

class SpriteAnimado : public QGraphicsPixmapItem {
public:
    SpriteAnimado(const QPixmap& tira, int cuadros, double fps,
                  const QSizeF& tamano, bool anclaPies);

    void cambiarTira(const QPixmap& nuevaTira, int nuevosCuadros, double nuevosFps);
    void avanzar(double deltaTiempo);
    void fijarCuadro(int indice);
    void fijarTamano(const QSizeF& nuevoTamano);
    void espejar(bool alReves);

private:
    void actualizarPixmap();

    QPixmap tira;
    int cuadros;
    double fps;
    QSizeF tamano;
    bool anclaPies;
    bool espejado = false;
    int cuadroActual = 0;
    double acumulado = 0.0;
};