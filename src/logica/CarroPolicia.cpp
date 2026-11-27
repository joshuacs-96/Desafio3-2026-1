
#include "CarroPolicia.h"

#include <QLineF>
#include <algorithm>

#include "CarroJugador.h"

namespace {
constexpr double X_APARICION = -100.0;
constexpr double DISTANCIA_NORMAL = 145.0;
constexpr double DISTANCIA_EMBESTIDA = 25.0;
constexpr double DISTANCIA_RETIRADA = 185.0;

constexpr double VELOCIDAD_NORMAL = 65.0;
constexpr double VELOCIDAD_EMBESTIDA = 210.0;
constexpr double VELOCIDAD_RETIRADA = 130.0;
constexpr double VELOCIDAD_LATERAL = 95.0;

constexpr double PRIMERA_ESPERA = 3.0;
}

CarroPolicia::CarroPolicia()
    : posicion(X_APARICION, 0.0),
    activo(false),
    enPantalla(false),
    yaGolpeo(false),
    cercaDelJugador(false),
    retrocediendo(false),
    embistiendo(false),
    radioSensor(70.0),
    tiempoMaximoCerca(0.4),
    tiempoCerca(0.0),
    velocidadX(65.0),
    velocidadLateral(VELOCIDAD_LATERAL),
    pausaEntrePasadas(7.0),
    tiempoEspera(PRIMERA_ESPERA) {
}

void CarroPolicia::configurar(bool nuevoActivo, double nuevoRadio,
                              double nuevoTiempoMaximo,
                              double nuevaVelocidadX, double nuevaPausa) {
    activo = nuevoActivo;
    radioSensor = nuevoRadio;
    tiempoMaximoCerca = nuevoTiempoMaximo;
    velocidadX = nuevaVelocidadX;
    pausaEntrePasadas = nuevaPausa;
    reiniciar();
}

void CarroPolicia::reiniciar() {
    posicion = QPointF(X_APARICION, 0.0);
    enPantalla = false;
    yaGolpeo = false;
    cercaDelJugador = false;
    retrocediendo = false;
    embistiendo = false;
    tiempoCerca = 0.0;
    tiempoEspera = PRIMERA_ESPERA;
}

void CarroPolicia::aparecer(const CarroJugador& jugador) {
    posicion = QPointF(X_APARICION, jugador.obtenerPosicion().y());
    enPantalla = true;
    yaGolpeo = false;
    retrocediendo = false;
    embistiendo = false;
    tiempoCerca = 0.0;
}

void CarroPolicia::activarEmbestida() {
    if (!activo || !enPantalla || embistiendo || retrocediendo) {
        return;
    }

    embistiendo = true;
    tiempoCerca = 0.0;
    yaGolpeo = false;
}

void CarroPolicia::actualizar(const CarroJugador& jugador,
                              double deltaTiempo) {
    if (!activo || deltaTiempo <= 0.0) {
        return;
    }

    if (!enPantalla) {
        tiempoEspera -= deltaTiempo;

        if (tiempoEspera <= 0.0) {
            aparecer(jugador);
        }
        return;
    }

    const QPointF posicionJugador = jugador.obtenerPosicion();

    // Sigue los cambios de carril del jugador.
    const double diferenciaY = posicionJugador.y() - posicion.y();
    const double pasoY = VELOCIDAD_LATERAL * deltaTiempo;

    posicion.setY(
        posicion.y() + std::clamp(diferenciaY, -pasoY, pasoY)
        );

    // Mantiene una distancia distinta según el estado.
    double distanciaObjetivo = DISTANCIA_NORMAL;
    double velocidadActual = velocidadX;

    if (embistiendo) {
        distanciaObjetivo = DISTANCIA_EMBESTIDA;
        velocidadActual = VELOCIDAD_EMBESTIDA;
    } else if (retrocediendo) {
        distanciaObjetivo = DISTANCIA_RETIRADA;
        velocidadActual = VELOCIDAD_RETIRADA;
    }

    const double objetivoX = posicionJugador.x() - distanciaObjetivo;
    const double diferenciaX = objetivoX - posicion.x();
    const double pasoX = velocidadActual * deltaTiempo;

    posicion.rx() += std::clamp(diferenciaX, -pasoX, pasoX);

    // Comprueba si la patrulla alcanzó al jugador.
    const QLineF distancia(posicion, posicionJugador);

    cercaDelJugador = embistiendo &&
                      distancia.length() <= radioSensor;

    if (cercaDelJugador && !yaGolpeo) {
        tiempoCerca += deltaTiempo;
    } else {
        tiempoCerca = 0.0;
    }

    // Después de retirarse, vuelve a seguir al jugador.
    if (retrocediendo &&
        posicion.x() <= objetivoX + 8.0) {
        retrocediendo = false;
    }
}

void CarroPolicia::registrarGolpe() {
    yaGolpeo = true;
    embistiendo = false;
    retrocediendo = true;
    cercaDelJugador = false;
    tiempoCerca = 0.0;
}

bool CarroPolicia::estaActivo() const {
    return activo;
}

bool CarroPolicia::estaEnPantalla() const {
    return enPantalla;
}

bool CarroPolicia::estaCercaDelJugador() const {
    return cercaDelJugador;
}

bool CarroPolicia::debeGolpear() const {
    return embistiendo && enPantalla && !yaGolpeo &&
           tiempoCerca >= tiempoMaximoCerca;
}

QPointF CarroPolicia::obtenerPosicion() const {
    return posicion;
}

double CarroPolicia::obtenerRadioSensor() const {
    return radioSensor;
}

double CarroPolicia::obtenerTiempoCerca() const {
    return tiempoCerca;
}
