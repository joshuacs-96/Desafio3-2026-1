#include "Nivel2Fuga.h"

#include <QLineF>
#include <algorithm>

Nivel2Fuga::Nivel2Fuga(bool modoDificil)
    : carroPolicia(QPointF(230.0, -180.0), 70.0, 3.0),
    modoDificil(modoDificil),
    tiempoRestante(20.0),
    velocidadScroll(100.0),
    limiteInferiorPantalla(500.0),
    danoPolicia(20) {
    iniciar();
}

void Nivel2Fuga::iniciar() {
    completado = false;
    perdido = false;
    configurarDificultad();

    carroJugador.establecerPosicion(QPointF(200.0, 380.0));
    carroJugador.detener();
    carroJugador.establecerBotinActual(3);
    carroPolicia.establecerPosicion(QPointF(230.0, -180.0));
    carroPolicia.reiniciarTiempoCerca();

    obstaculos.clear();
    obstaculos.append(Obstaculo("Aceite", QPointF(120.0, 100.0), 0, 2.0));
    obstaculos.append(Obstaculo("Hueco", QPointF(260.0, -40.0), 20, 0.0));
    obstaculos.append(Obstaculo("Barrera", QPointF(180.0, -180.0), 12, 0.0));
}

void Nivel2Fuga::actualizar(double deltaTiempo) {
    if (completado || perdido) {
        return;
    }

    carroJugador.actualizar(deltaTiempo);

    // el carro no se sale de la calzada
    QPointF p = carroJugador.obtenerPosicion();
    p.setX(std::clamp(p.x(), 50.0, 350.0));
    p.setY(std::max(p.y(), 120.0));
    carroJugador.establecerPosicion(p);

    desplazarPeligros(deltaTiempo);
    carroPolicia.actualizar(carroJugador, deltaTiempo);
    verificarColisiones();
    verificarLimitesPantalla();

    tiempoRestante -= deltaTiempo;

    if (tiempoRestante <= 0.0 && !perdido) {
        tiempoRestante = 0.0;
        completado = true;
    }

    if (carroJugador.estaDestruido()) {
        perdido = true;
    }
}

void Nivel2Fuga::acelerarCarro(double cantidad) {
    if (!completado && !perdido) {
        carroJugador.acelerar(cantidad);
    }
}

void Nivel2Fuga::frenarCarro(double cantidad) {
    if (!completado && !perdido) {
        carroJugador.frenar(cantidad);
    }
}

void Nivel2Fuga::girarCarro(double direccion) {
    if (!completado && !perdido) {
        carroJugador.girar(direccion);
    }
}

const CarroJugador& Nivel2Fuga::obtenerCarroJugador() const {
    return carroJugador;
}

const CarroPolicia& Nivel2Fuga::obtenerCarroPolicia() const {
    return carroPolicia;
}

double Nivel2Fuga::obtenerTiempoRestante() const {
    return tiempoRestante;
}

bool Nivel2Fuga::estaEnModoDificil() const {
    return modoDificil;
}

const QVector<Obstaculo>& Nivel2Fuga::obtenerObstaculos() const {
    return obstaculos;
}

double Nivel2Fuga::obtenerVelocidadScroll() const {
    return velocidadScroll;
}

void Nivel2Fuga::configurarDificultad() {
    if (modoDificil) {
        tiempoRestante = 20.0;
        velocidadScroll = 105.0;
        danoPolicia = 30;
        carroJugador.establecerIntegridad(70.0);
        carroPolicia.configurarSensor(110.0, 1.5);
        return;
    }

    tiempoRestante = 20.0;
    velocidadScroll = 70.0;
    danoPolicia = 20;
    carroJugador.establecerIntegridad(100.0);
    carroPolicia.configurarSensor(70.0, 3.0);
}

void Nivel2Fuga::desplazarPeligros(double deltaTiempo) {
    double desplazamiento = velocidadScroll * deltaTiempo;

    carroPolicia.moverHorizontalmente(desplazamiento);

    for (Obstaculo& obstaculo : obstaculos) {
        obstaculo.moverHorizontalmente(-desplazamiento);
    }
}
void Nivel2Fuga::verificarColisiones() {
    for (Obstaculo& obstaculo : obstaculos) {
        if (!obstaculo.estaActivo()) {
            continue;
        }

        QLineF distancia(carroJugador.obtenerPosicion(), obstaculo.obtenerPosicion());

        if (distancia.length() <= obstaculo.obtenerRadioColision()) {
            obstaculo.aplicarEfecto(carroJugador);
        }
    }

    if (carroPolicia.superoTiempoCerca()) {
        carroJugador.recibirDanio(danoPolicia);
        carroJugador.perderBotin(1);
        carroPolicia.reiniciarTiempoCerca();
    }
}

void Nivel2Fuga::verificarLimitesPantalla() {
    if (carroJugador.obtenerPosicion().y() >= limiteInferiorPantalla) {
        perdido = true;
    }
}
