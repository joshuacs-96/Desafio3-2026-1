#include "Edificio.h"

#include <cmath>
#include <algorithm>

void Edificio::configurar(const QVector<double>& alturasPisos,
                          const QVector<double>& columnasEscaleras) {
    pisos = alturasPisos;
    columnas = columnasEscaleras;
    listaEscaleras.clear();

    // En cada hueco hay una escalera de subida y otra de bajada por cada tramo
    for (int i = 0; i + 1 < pisos.size(); ++i) {
        for (int j = 0; j < columnas.size(); ++j) {
            if (columnas.size() == 4 && j / 2 != i) continue;
            double x = columnas[j];
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

void Edificio::actualizarLasers(bool soporteActivo, bool bovedaCerrada) {
    laserSoporteActivo = soporteActivo;
    lasersBovedaActivos = bovedaCerrada;
}

void Edificio::retirarEscaleraDerechaInferior() {
    for (int i = listaEscaleras.size() - 1; i >= 0; --i) {
        const Escalera& escalera = listaEscaleras[i];
        if (escalera.obtenerPosicion().x() == 320.0
            && std::max(escalera.obtenerPosicion().y(), escalera.obtenerDestino().y()) == pisos.value(0)) {
            listaEscaleras.removeAt(i);
        }
    }
}

QVector<QPointF> Edificio::planificarRuta(const QPointF& origen, const QPointF& destino,
                                         bool usarAlternativa) const {
    // Nodos: origen, destino y los extremos de cada escalera.
    QVector<QPointF> nodos;
    nodos.append(QPointF(origen.x(), pisos.value(pisoDe(origen))));
    nodos.append(QPointF(destino.x(), pisos.value(pisoDe(destino))));
    for (const Escalera& escalera : listaEscaleras) {
        if (!nodos.contains(escalera.obtenerPosicion())) nodos.append(escalera.obtenerPosicion());
        if (!nodos.contains(escalera.obtenerDestino())) nodos.append(escalera.obtenerDestino());
    }

    const int cantidad = nodos.size();
    QVector<double> distancia(cantidad, 1e12);
    QVector<int> anterior(cantidad, -1);
    QVector<bool> visitado(cantidad, false);
    distancia[0] = 0.0;

    for (int paso = 0; paso < cantidad; ++paso) {
        int actual = -1;
        for (int i = 0; i < cantidad; ++i) {
            if (!visitado[i] && (actual < 0 || distancia[i] < distancia[actual])) actual = i;
        }
        if (actual < 0 || distancia[actual] >= 1e12) break;
        visitado[actual] = true;

        for (int siguiente = 0; siguiente < cantidad; ++siguiente) {
            if (visitado[siguiente] || siguiente == actual) continue;
            const QPointF a = nodos[actual];
            const QPointF b = nodos[siguiente];
            const int pisoA = pisoDe(a);
            const int pisoB = pisoDe(b);
            bool conectado = false;
            double costo = 0.0;

            if (pisoA == pisoB) {
                conectado = true;
                const double lasers[] = {250.0, 355.0, 350.0};
                for (int i = 0; i < 3; ++i) {
                    const bool activo = i == 0 ? laserSoporteActivo : lasersBovedaActivos;
                    const int pisoLaser = i == 2 ? 1 : 2;
                    if (activo && pisoA == pisoLaser
                        && (a.x() - lasers[i]) * (b.x() - lasers[i]) <= 0.0
                        && a.x() != b.x()) conectado = false;
                }
                costo = std::abs(a.x() - b.x());
            } else {
                for (const Escalera& escalera : listaEscaleras) {
                    if (escalera.obtenerPosicion() == a && escalera.obtenerDestino() == b) {
                        conectado = true;
                        costo = std::abs(a.y() - b.y());
                        // En modo alternativo se penaliza una escalera, sin cruzar obstáculos.
                        if (usarAlternativa && a.x() < 200.0) costo += 150.0;
                        break;
                    }
                }
            }

            if (conectado && distancia[actual] + costo < distancia[siguiente]) {
                distancia[siguiente] = distancia[actual] + costo;
                anterior[siguiente] = actual;
            }
        }
    }

    QVector<QPointF> ruta;
    if (distancia[1] >= 1e12) return ruta;
    for (int nodo = 1; nodo != 0; nodo = anterior[nodo]) {
        if (nodo < 0) return {};
        ruta.prepend(nodos[nodo]);
    }
    return ruta;
}
