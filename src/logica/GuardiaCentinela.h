#pragma once

#include <QMap>
#include <QPointF>
#include <QString>
#include <QVector>

class GuardiaCentinela {
public:
    GuardiaCentinela(const QPointF& posicionInicial, double radioPercepcion);

    bool percibir(const QPointF& posicionRuido, double intensidad);
    void decidir();
    void actuar(double deltaTiempo);
    void registrarZona(const QString& zona, const QPointF& posicionZona);
    void aprender(const QString& zona, bool encontroMinion);

    QString obtenerEstadoActual() const;
    QPointF obtenerPosicion() const;
    double consultarZona(const QString& zona) const;
    QString obtenerZonaMasSospechosa() const;

private:
    QPointF posicion;
    double radioPercepcion;
    QString estadoActual;
    QPointF ultimaPosicionRuido;
    QMap<QString, double> mapaCalor;
    QMap<QString, QPointF> posicionesZonas;
    double tasaAprendizaje;
    double umbralInvestigacion;
    bool hayRuidoPendiente;
    QVector<QPointF> rutaPatrulla;
    int indiceRutaActual;
    double velocidad;
};
