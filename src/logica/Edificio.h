#pragma once

#include <QPointF>
#include <QVector>

#include "Escalera.h"

class Edificio {
public:
    // alturasPisos: y de cada piso, del piso 1 (abajo, y mayor) al último (arriba, y menor)
    // columnasEscaleras: x de cada hueco de escalera (cada uno conecta todos los pisos)
    void configurar(const QVector<double>& alturasPisos,
                    const QVector<double>& columnasEscaleras);

    int cantidadPisos() const;
    int pisoDe(const QPointF& posicion) const;      // índice 0 = piso 1
    double alturaDelPiso(int piso) const;

    const QVector<Escalera>& escaleras() const;

    // Puntos por los que hay que pasar para ir de origen a destino.
    // Dos puntos seguidos con la misma x y distinta y significan "usar la escalera".
    QVector<QPointF> planificarRuta(const QPointF& origen, const QPointF& destino) const;

private:
    QVector<double> pisos;
    QVector<double> columnas;
    QVector<Escalera> listaEscaleras;
};