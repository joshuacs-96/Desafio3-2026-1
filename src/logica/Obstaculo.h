#pragma once

#include <QPointF>
#include <QString>

class Minion;
class CarroJugador;

class Obstaculo {
public:
    Obstaculo(const QString& tipo, const QPointF& posicion, int dano, double factorVelocidad);

    void aplicarEfecto(Minion& minion);
    void aplicarEfecto(CarroJugador& carro);
    void moverVerticalmente(double desplazamiento);

    QString obtenerTipo() const;
    QPointF obtenerPosicion() const;
    double obtenerRadioColision() const;
    bool estaActivo() const;

private:
    QPointF posicion;
    QString tipo;
    int dano;
    double factorVelocidad;
    double radioColision;
    bool activo;
};
