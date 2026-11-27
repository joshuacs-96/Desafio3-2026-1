// Nivel2Fuga.cpp
#include "Nivel2Fuga.h"

#include <QLineF>
#include <QRandomGenerator>
#include <algorithm>

namespace {
// Coordenadas lógicas (AJÚSTALAS con el clic sobre tu fondo)
constexpr double CARRIL_Y[3] = {
    80.80,   // carril superior
    186.80,  // carril central
    289.01   // carril inferior
};
constexpr double X_CARRO = 180.0;                     // el carro no avanza: se mueve la carretera
constexpr double MARGEN_CARRIL = 25.0;                   // cuánto puede salirse de los carriles
constexpr double X_APARICION = 700.0;                    // los obstáculos nacen fuera de pantalla, a la derecha
constexpr double X_DESAPARICION = -80.0;                 // y se eliminan al salir por la izquierda
constexpr double DURACION_NIVEL = 60.0;                  // segundos que hay que aguantar
}

Nivel2Fuga::Nivel2Fuga(bool modoDificil)
    : modoDificil(modoDificil),
    tiempoRestante(DURACION_NIVEL),
    velocidadScroll(70.0),
    intervaloObstaculos(1.5),
    tiempoParaObstaculo(1.0),
    danoPolicia(0),
    ultimoCarril(1),
    tiempoParaRecolectable(4.0){
    iniciar();
}

void Nivel2Fuga::iniciar() {
    completado = false;
    perdido = false;
    configurarDificultad();

    carroJugador.detener();
    carroJugador.establecerPosicion(QPointF(X_CARRO, CARRIL_Y[1]));   // empieza en el carril del medio
    carroJugador.establecerBotinActual(3);

    obstaculos.clear();
    recolectables.clear();
    tiempoParaRecolectable = 4.0;
    tiempoParaObstaculo = 0.2;
    ultimoCarril = 1;
}

void Nivel2Fuga::configurarDificultad() {
    tiempoRestante = DURACION_NIVEL;
    carroJugador.establecerIntegridad(100.0);

    if (modoDificil) {
        velocidadScroll = 105.0;
        intervaloObstaculos = 0.5;
        danoPolicia = 15;

        carroPolicia.configurar(true, 65.0, 0.4, 65.0, 7.0);
        return;
    }

    // Fácil: solo esquivar obstáculos, sin patrulla
    velocidadScroll = 70.0;
    intervaloObstaculos = 1;
    danoPolicia = 0;
    carroPolicia.configurar(false, 80.0, 0.8, 130.0, 8.0);
}

void Nivel2Fuga::actualizar(double deltaTiempo) {
    if (completado || perdido) {
        return;
    }

    carroJugador.actualizar(deltaTiempo);

    // el carro solo se mueve entre los carriles (arriba/abajo): su x es fija
    QPointF p = carroJugador.obtenerPosicion();
    p.setX(X_CARRO);
    p.setY(std::clamp(p.y(), CARRIL_Y[0] - MARGEN_CARRIL, CARRIL_Y[2] + MARGEN_CARRIL));
    carroJugador.establecerPosicion(p);

    // aparecen obstáculos todo el tiempo
    tiempoParaObstaculo -= deltaTiempo;
    if (tiempoParaObstaculo <= 0.0) {
        generarObstaculo();
        tiempoParaObstaculo = intervaloObstaculos * (0.8 + 0.4 * QRandomGenerator::global()->generateDouble());
    }

    // Aparición periódica de objetos recolectables
    tiempoParaRecolectable -= deltaTiempo;

    if (tiempoParaRecolectable <= 0.0) {
        generarRecolectable();
        tiempoParaRecolectable = 5.0 + QRandomGenerator::global()->generateDouble() * 3.0;
    }

    desplazarObstaculos(deltaTiempo);
    desplazarRecolectables(deltaTiempo);
    carroPolicia.actualizar(carroJugador, deltaTiempo);
    verificarColisiones();

    if (carroJugador.estaDestruido()) {
        perdido = true;
        return;
    }

    tiempoRestante -= deltaTiempo;
    if (tiempoRestante <= 0.0) {
        tiempoRestante = 0.0;
        completado = true;       // aguantó todo el tiempo: gana
    }
}

void Nivel2Fuga::moverCarro(double direccion) {
    if (!completado && !perdido) {
        carroJugador.cambiarCarril(direccion);
    }
}

void Nivel2Fuga::generarObstaculo() {
    QRandomGenerator* azar = QRandomGenerator::global();

    // carril al azar, procurando que no sea siempre el mismo
    int carril = azar->bounded(3);
    if (carril == ultimoCarril && azar->bounded(100) < 60) {
        carril = (carril + 1 + azar->bounded(2)) % 3;
    }
    ultimoCarril = carril;

    // tipo al azar: tipo, daño y (en el aceite) duración del derrape
    const int sorteo = azar->bounded(100);
    QString tipo;
    int dano = 0;
    double derrape = 0.0;

    if (sorteo < 30) {
        tipo = "Cono";
        dano = 8;
    } else if (sorteo < 55) {
        tipo = "Aceite";
        derrape = 2.0;
    } else if (sorteo < 80) {
        tipo = "Hueco";
        dano = 15;
    } else {
        tipo = "Barrera";
        dano = 12;
    }

    obstaculos.append(Obstaculo(tipo, QPointF(X_APARICION, CARRIL_Y[carril]), dano, derrape));
}

void Nivel2Fuga::desplazarObstaculos(double deltaTiempo) {
    const double desplazamiento = velocidadScroll * deltaTiempo;

    for (Obstaculo& obstaculo : obstaculos) {
        obstaculo.moverHorizontalmente(-desplazamiento);      // la carretera se mueve hacia la izquierda
    }

    // Salen en el mismo orden en que aparecen, así que basta mirar el primero
    while (!obstaculos.isEmpty() && obstaculos.first().obtenerPosicion().x() < X_DESAPARICION) {
        obstaculos.removeFirst();
    }
}

void Nivel2Fuga::verificarColisiones() {
    for (Obstaculo& obstaculo : obstaculos) {
        if (!obstaculo.estaActivo()) {
            continue;
        }

        QLineF distancia(carroJugador.obtenerPosicion(), obstaculo.obtenerPosicion());

        if (distancia.length() <= obstaculo.obtenerRadioColision()) {

            // La patrulla solo aprovecha el choque si no hay escudo.
            if (!carroJugador.estaProtegido()) {
                carroPolicia.activarEmbestida();
            }

            obstaculo.aplicarEfecto(carroJugador);
        }
    }

    if (carroPolicia.debeGolpear()) {
        if (!carroJugador.estaProtegido()) {
            carroJugador.recibirDanio(danoPolicia);
            carroJugador.perderBotin(1);
        }

        carroPolicia.registrarGolpe();
    }
    verificarColisionesRecolectables();
}

void Nivel2Fuga::generarRecolectable() {
    const int carril = QRandomGenerator::global()->bounded(3);

    // Mitad de probabilidad para cada objeto
    const QString tipo = QRandomGenerator::global()->bounded(2) == 0
                             ? "Banana" : "Lingote";

    recolectables.append({
        tipo,
        QPointF(X_APARICION, CARRIL_Y[carril]),
        true
    });
}

void Nivel2Fuga::desplazarRecolectables(double deltaTiempo) {
    const double desplazamiento = velocidadScroll * deltaTiempo;

    for (RecolectableNivel2& item : recolectables) {
        item.posicion.rx() -= desplazamiento;
    }

    while (!recolectables.isEmpty()
           && recolectables.first().posicion.x() < X_DESAPARICION) {
        recolectables.removeFirst();
    }
}

void Nivel2Fuga::verificarColisionesRecolectables() {
    for (RecolectableNivel2& item : recolectables) {
        if (!item.activo) {
            continue;
        }

        QLineF distancia(carroJugador.obtenerPosicion(), item.posicion);

        if (distancia.length() <= 35.0) {
            if (item.tipo == "Lingote") {
                carroJugador.recuperarIntegridad(10.0);
            } else if (item.tipo == "Banana") {
                carroJugador.activarEscudo(5.0);
            }

            item.activo = false;
        }
    }
}

const QVector<RecolectableNivel2>& Nivel2Fuga::obtenerRecolectables() const {
    return recolectables;
}

const CarroJugador& Nivel2Fuga::obtenerCarroJugador() const { return carroJugador; }
const CarroPolicia& Nivel2Fuga::obtenerCarroPolicia() const { return carroPolicia; }
const QVector<Obstaculo>& Nivel2Fuga::obtenerObstaculos() const { return obstaculos; }
double Nivel2Fuga::obtenerTiempoRestante() const { return tiempoRestante; }
double Nivel2Fuga::obtenerVelocidadScroll() const { return velocidadScroll; }
bool Nivel2Fuga::estaEnModoDificil() const { return modoDificil; }