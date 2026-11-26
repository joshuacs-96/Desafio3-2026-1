#pragma once

#include <QMap>
#include <QPointF>
#include <QString>
#include <QVector>

class Edificio;

class GuardiaCentinela {
public:
    GuardiaCentinela(const QPointF& posicionInicial, double radioPercepcion);

    void establecerEdificio(const Edificio* nuevoEdificio);
    void activarMovimiento();
    void activarAlarma(const QPointF& lugar);
    void establecerPosicion(const QPointF& nuevaPosicion);

    bool percibir(const QPointF& posicionRuido, double intensidad);
    void ver(const QPointF& posicionMinion, bool usandoEscalera,
             const QPointF& destinoEscalera);
    void decidir();
    void actuar(double deltaTiempo);
    void registrarZona(const QString& zona, const QPointF& posicionZona);
    void aprender(const QString& zona, bool encontroMinion);
    void reiniciar(const QPointF& nuevaPosicion);

    QString obtenerEstadoActual() const;
    QPointF obtenerPosicion() const;
    double consultarZona(const QString& zona) const;
    QString obtenerZonaMasSospechosa() const;

private:
    void planificar(const QPointF& destino);
    bool estaEnEscalera() const;
    void limpiarRutaConservandoEscalera();
    void avanzarPorRuta(double deltaTiempo, double velocidadActual);
    QString elegirZonaDePatrulla() const;

    QPointF posicion;
    double radioPercepcion;
    QString estadoActual;
    QPointF ultimaPosicionRuido;
    QMap<QString, double> mapaCalor;
    QMap<QString, QPointF> posicionesZonas;
    double tasaAprendizaje;
    bool hayRuidoPendiente;
    double velocidad;
    QPointF ultimaPosicionVista;

    const Edificio* edificio = nullptr;      // no es dueño: lo posee el nivel
    QVector<QPointF> ruta;
    double radioVision = 120.0;
    bool viendoMinion = false;
    double tiempoDesdeUltimaVista = 0.0;
    double espera = 0.0;
    bool movimientoActivado = false;
    bool patrullaInicialPendiente = true;
    double probabilidadRutaOptima = 0.70;
    bool usarRutaAlternativa = false;
    bool alarmaActiva = false;
};
