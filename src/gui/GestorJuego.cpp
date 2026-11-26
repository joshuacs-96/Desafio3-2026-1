#include "GestorJuego.h"

#include <QLineF>
#include <algorithm>

#include "ConfigGrafica.h"

GestorJuego::GestorJuego(QObject* padre) : QObject(padre) {
    connect(&temporizador, &QTimer::timeout, this, &GestorJuego::tick);
}

void GestorJuego::iniciarJuego() {
    iniciarNivel(1);
}

void GestorJuego::establecerDificultad(bool dificil) {
    modoDificil = dificil;
}

void GestorJuego::iniciarNivel(int numero) {
    temporizador.stop();
    teclas.clear();
    caminando = false;
    golpeando = false;
    miraIzquierda = false;
    capturaEnCurso = false;
    tiempoCaptura = 0.0;
    tiempoMartillo = 0.0;

    if (numero == 1) {
        n2.reset();
        n1 = std::make_unique<Nivel1Banco>();
        actual = n1.get();
    } else {
        n1.reset();
        n2 = std::make_unique<Nivel2Fuga>(modoDificil);
        actual = n2.get();
    }
    nivelEnCurso = numero;

    emit nivelIniciado(numero);
    reloj.start();
    temporizador.start(16);              // ~60 cuadros por segundo
}

void GestorJuego::avanzar() {
    if (nivelEnCurso == 1) {
        iniciarNivel(2);
    } else {
        emit juegoGanado();
    }
}

void GestorJuego::teclaPresionada(int codigo) {
    // ATAJOS DE PRUEBA (bórralos al final):
    // F1 abre la puerta de la bóveda, F2 = nivel 2 normal, F3 = nivel 2 difícil
    if (codigo == Qt::Key_F2 || codigo == Qt::Key_F3) {
        modoDificil = (codigo == Qt::Key_F3);
        iniciarNivel(2);
        return;
    }
    if (codigo == Qt::Key_F1 && nivelEnCurso == 1 && n1) {
        n1->danarPuerta(1000.0);
        return;
    }

    teclas.insert(codigo);

    if (nivelEnCurso != 1 || !n1 || n1->estaCompletado() || n1->estaPerdido()) {
        return;
    }

    if (codigo == Qt::Key_W || codigo == Qt::Key_Up) {
        if (!n1->usarEscalera(true)) {          // sube si hay escalera; si no, salta
            n1->saltarMinion();
        }
    } else if (codigo == Qt::Key_S || codigo == Qt::Key_Down) {
        n1->usarEscalera(false);                // baja si hay escalera
    }
}

void GestorJuego::teclaSoltada(int codigo) {
    teclas.remove(codigo);
}

void GestorJuego::tick() {
    if (!actual) {
        return;
    }

    const double dt = std::min(0.05, reloj.restart() / 1000.0);

    // Durante la captura se congelan los movimientos y la lógica del nivel.
    if (capturaEnCurso) {
        tiempoCaptura -= dt;

        emit estadoActualizado(dt);

        if (tiempoCaptura <= 0.0) {
            tiempoCaptura = 0.0;

            if (nivelEnCurso == 1 && n1) {
                n1->finalizarCaptura();
            }

            capturaEnCurso = false;

            if (actual && actual->estaPerdido()) {
                temporizador.stop();
                emit nivelPerdido(nivelEnCurso);
            }
        }

        return;
    }

    if (nivelEnCurso == 1) {
        tickNivel1(dt);
    } else {
        tickNivel2(dt);
    }

    emit estadoActualizado(dt);

    // No declarar la derrota mientras se muestran las animaciones.
    if (capturaEnCurso) {
        return;
    }

    if (actual->estaCompletado()) {
        temporizador.stop();
        emit nivelCompletado(nivelEnCurso);
    } else if (actual->estaPerdido()) {
        temporizador.stop();
        emit nivelPerdido(nivelEnCurso);
    }
}

void GestorJuego::tickNivel1(double dt) {
    const bool puertaAntes = n1->obtenerBoveda().estaAbierta();
    const int vidasAntes = n1->obtenerMinion().obtenerVidas();
    const int botinAntes = n1->obtenerBotinRecolectado();
    const bool bananaAntes = n1->obtenerMinion().tieneBananaActiva();
    const QString estadoAntes = n1->obtenerGuardia().obtenerEstadoActual();

    // --- entrada del teclado ---
    const double direccion =
        ((estaPresionada(Qt::Key_D) || estaPresionada(Qt::Key_Right)) ? 1.0 : 0.0)
        - ((estaPresionada(Qt::Key_A) || estaPresionada(Qt::Key_Left)) ? 1.0 : 0.0);

    caminando = direccion != 0.0;
    if (caminando) {
        miraIzquierda = direccion < 0.0;
        n1->moverMinion(QPointF(direccion, 0.0), dt);
    }

    golpeando = estaPresionada(Qt::Key_Space) && n1->cercaDeObjetivo();
    if (golpeando) {
        tiempoMartillo += dt;
        if (tiempoMartillo >= 0.5) {
            tiempoMartillo -= 0.5;
            n1->golpear();
        }
    } else {
        tiempoMartillo = 0.0;
    }

    n1->actualizar(dt);

    // la lógica no emite señales: aquí se traducen los cambios a señales de Qt
    if (!puertaAntes && n1->obtenerBoveda().estaAbierta()) {
        emit puertaAbierta();
    }
    if (n1->obtenerBotinRecolectado() > botinAntes) {
        emit botinRecogido(n1->obtenerBotinRecolectado());
    }
    if (!bananaAntes && n1->obtenerMinion().tieneBananaActiva()) {
        emit bananaUsada();
    }
    if (n1->obtenerMinion().obtenerVidas() < vidasAntes) {
        capturaEnCurso = true;
        tiempoCaptura = 2.0;

        emit minionCapturado(n1->obtenerMinion().obtenerVidas());
    }
    const QString estadoAhora = n1->obtenerGuardia().obtenerEstadoActual();
    if (estadoAhora != estadoAntes) {
        emit guardiaCambioEstado(estadoAhora);
    }
}

void GestorJuego::tickNivel2(double dt) {
    constexpr double ACELERACION = 240.0;

    if (estaPresionada(Qt::Key_W) || estaPresionada(Qt::Key_Up)) {
        n2->acelerarCarro(ACELERACION * dt);
    }
    if (estaPresionada(Qt::Key_S) || estaPresionada(Qt::Key_Down)) {
        n2->frenarCarro(ACELERACION * dt);
    }

    const double direccion =
        ((estaPresionada(Qt::Key_D) || estaPresionada(Qt::Key_Right)) ? 1.0 : 0.0)
        - ((estaPresionada(Qt::Key_A) || estaPresionada(Qt::Key_Left)) ? 1.0 : 0.0);

    // al soltar las teclas el carro deja de girar (salvo si está derrapando)
    if (direccion != 0.0 || !n2->obtenerCarroJugador().estaDerrapando()) {
        n2->girarCarro(direccion);
    }

    const double integridadAntes = n2->obtenerCarroJugador().obtenerIntegridad();
    n2->actualizar(dt);

    if (n2->obtenerCarroJugador().obtenerIntegridad() < integridadAntes) {
        emit carroGolpeado();
    }
}

bool GestorJuego::estaPresionada(int codigo) const {
    return teclas.contains(codigo);
}

bool GestorJuego::cercaDeLaPuerta() const {
    if (!n1) {
        return false;
    }
    const QLineF distancia(n1->obtenerMinion().obtenerPosicionSuelo(), n1->obtenerPosicionPuerta());
    return distancia.length() <= ConfigGrafica::RADIO_INTERACCION_PUERTA;
}

int GestorJuego::numeroNivel() const { return nivelEnCurso; }
const Nivel1Banco* GestorJuego::nivel1() const { return n1.get(); }
const Nivel2Fuga* GestorJuego::nivel2() const { return n2.get(); }
bool GestorJuego::minionCaminando() const { return caminando; }
bool GestorJuego::minionMiraIzquierda() const { return miraIzquierda; }
bool GestorJuego::minionGolpeando() const { return golpeando; }
int GestorJuego::faseCaptura() const {
    if (!capturaEnCurso) {
        return 0;
    }

    if (tiempoCaptura > 1.4) {
        return 1;  // Minion capturado
    }

    return 2;      // Minion golpeado
}
