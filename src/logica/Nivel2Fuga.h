#pragma once

#include <QVector>

#include "CarroJugador.h"
#include "CarroPolicia.h"
#include "Nivel.h"
#include "Obstaculo.h"

class Nivel2Fuga : public Nivel {
public:
    explicit Nivel2Fuga(bool modoDificil = false);

    void iniciar() override;
    void actualizar(double deltaTiempo) override;

    void acelerarCarro(double cantidad);
    void frenarCarro(double cantidad);
    void girarCarro(double direccion);

    const CarroJugador& obtenerCarroJugador() const;
    const CarroPolicia& obtenerCarroPolicia() const;
    double obtenerTiempoRestante() const;
    bool estaEnModoDificil() const;

private:
    void configurarDificultad();
    void desplazarPeligros(double deltaTiempo);
    void verificarColisiones();
    void verificarLimitesPantalla();

    CarroJugador carroJugador;
    CarroPolicia carroPolicia;
    QVector<Obstaculo> obstaculos;
    bool modoDificil;
    double tiempoRestante;
    double velocidadScroll;
    double limiteInferiorPantalla;
    int danoPolicia;
};
