#pragma once

#include <QPointF>
#include <QString>

class Minion;
class CarroJugador;

class Obstaculo {
public:
    Obstaculo(const QString& tipo, const QPointF& posicion, int dano, double factorVelocidad);

    void aplicarEfecto(Minion& minion) const;
    void aplicarEfecto(CarroJugador& carro) const;

    QString obtenerTipo() const;
    QPointF obtenerPosicion() const;

private:
    QPointF posicion;
    QString tipo;
    int dano;
    double factorVelocidad;
};
