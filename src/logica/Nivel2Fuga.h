// Nivel2Fuga.h
#pragma once

#include <QVector>
#include <QString>
#include <QPointF>

#include "CarroJugador.h"
#include "CarroPolicia.h"
#include "Nivel.h"
#include "Obstaculo.h"

struct RecolectableNivel2 {
    QString tipo;
    QPointF posicion;
    bool activo = true;
};

class Nivel2Fuga : public Nivel {
public:
    explicit Nivel2Fuga(bool modoDificil = false);

    void iniciar() override;
    void actualizar(double deltaTiempo) override;

    void moverCarro(double direccion);       // -1 = hacia el carril de arriba, +1 = hacia el de abajo

    const CarroJugador& obtenerCarroJugador() const;
    const CarroPolicia& obtenerCarroPolicia() const;
    const QVector<Obstaculo>& obtenerObstaculos() const;
    double obtenerTiempoRestante() const;
    double obtenerVelocidadScroll() const;
    bool estaEnModoDificil() const;
    const QVector<RecolectableNivel2>& obtenerRecolectables() const;

private:
    void configurarDificultad();
    void generarObstaculo();
    void desplazarObstaculos(double deltaTiempo);
    void verificarColisiones();
    void generarRecolectable();
    void desplazarRecolectables(double deltaTiempo);
    void verificarColisionesRecolectables();

    CarroJugador carroJugador;
    CarroPolicia carroPolicia;

    QVector<Obstaculo> obstaculos;
    QVector<RecolectableNivel2> recolectables;

    bool modoDificil;
    double tiempoRestante;
    double velocidadScroll;
    double intervaloObstaculos;
    double tiempoParaObstaculo;
    int danoPolicia;
    int ultimoCarril;
    double tiempoParaRecolectable;
};