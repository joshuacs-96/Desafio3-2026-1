#include "Edificio.h"

#include <cmath>

void Edificio::configurar(const QVector<double>& alturasPisos,
                          const QVector<double>& columnasEscaleras) {
    pisos = alturasPisos;
    columnas = columnasEscaleras;
    listaEscaleras.clear();

    // En cada hueco hay una escalera de subida y otra de bajada por cada tramo
    for (double x : columnas) {
        for (int i = 0; i + 1 < pisos.size(); ++i) {
            listaEscaleras.append(Escalera(QPointF(x, pisos[i]), QPointF(x, pisos[i + 1])));  // sube
            listaEscaleras.append(Escalera(QPointF(x, pisos[i + 1]), QPointF(x, pisos[i])));  // baja
        }
    }
}

int Edificio::cantidadPisos() const {
    return pisos.size();
}

int Edificio::pisoDe(const QPointF& posicion) const {
    int mejor = 0;
    double menorDistancia = std::abs(posicion.y() - pisos.value(0));

    for (int i = 1; i < pisos.size(); ++i) {
        const double distancia = std::abs(posicion.y() - pisos[i]);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            mejor = i;
        }
    }

    return mejor;
}

double Edificio::alturaDelPiso(int piso) const {
    return pisos.value(piso);
}

const QVector<Escalera>& Edificio::escaleras() const {
    return listaEscaleras;
}

QVector<QPointF> Edificio::planificarRuta(const QPointF& origen, const QPointF& destino) const {
    QVector<QPointF> ruta;

    if (pisos.isEmpty() || columnas.isEmpty()) {
        ruta.append(destino);
        return ruta;
    }

    int pisoActual = pisoDe(origen);
    const int pisoDestino = pisoDe(destino);
    double xActual = origen.x();

    while (pisoActual != pisoDestino) {
        const int siguiente = pisoActual + (pisoDestino > pisoActual ? 1 : -1);

        // elige el hueco de escalera que menos recorrido horizontal exige
        double mejorX = columnas.first();
        double menorCosto = 1e9;
        for (double x : columnas) {
            const double costo = std::abs(xActual - x) + std::abs(x - destino.x());
            if (costo < menorCosto) {
                menorCosto = costo;
                mejorX = x;
            }
        }

        ruta.append(QPointF(mejorX, pisos[pisoActual]));     // camina hasta la escalera
        ruta.append(QPointF(mejorX, pisos[siguiente]));      // la usa
        xActual = mejorX;
        pisoActual = siguiente;
    }

    ruta.append(QPointF(destino.x(), pisos[pisoDestino]));
    return ruta;
}