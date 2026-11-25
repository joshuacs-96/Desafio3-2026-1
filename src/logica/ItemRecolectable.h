#pragma once

#include <QPointF>
#include <QString>

class ItemRecolectable {
public:
    // tipo: "Banana", "Lingote" o "Bolsa"
    ItemRecolectable(const QString& tipo, const QPointF& posicion, int valor, bool dentroDeBoveda);

    QString obtenerTipo() const;
    QPointF obtenerPosicion() const;
    int obtenerValor() const;
    bool estaDentroDeBoveda() const;
    bool fueRecogido() const;
    void recoger();

private:
    QString tipo;
    QPointF posicion;
    int valor;
    bool dentroDeBoveda;
    bool recogido;
};