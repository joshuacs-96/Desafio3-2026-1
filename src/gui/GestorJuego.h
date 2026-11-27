#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QSet>
#include <QTimer>
#include <memory>

#include "logica/Nivel1Banco.h"
#include "logica/Nivel2Fuga.h"

class GestorJuego : public QObject {
    Q_OBJECT
public:
    explicit GestorJuego(QObject* padre = nullptr);

    void iniciarJuego();
    void establecerDificultad(bool dificil);

    int numeroNivel() const;
    const Nivel1Banco* nivel1() const;
    const Nivel2Fuga* nivel2() const;

    bool minionCaminando() const;
    bool minionMiraIzquierda() const;
    bool minionGolpeando() const;
    bool cercaDeLaPuerta() const;
    int faseCaptura() const;

public slots:
    void iniciarNivel(int numero);
    void avanzar();                      // pasa al nivel 2 o termina el juego
    void teclaPresionada(int codigo);
    void teclaSoltada(int codigo);

signals:
    void nivelIniciado(int numero);
    void estadoActualizado(double deltaTiempo);
    void puertaAbierta();
    void botinRecogido(int total);
    void minionCapturado(int vidasRestantes);
    void guardiaCambioEstado(const QString& estado);
    void bananaUsada();
    void minionRio();
    void golpeRealizado(int golpesConsecutivos);
    void carroGolpeado();
    void nivelCompletado(int numero);
    void nivelPerdido(int numero);
    void juegoGanado();

private slots:
    void tick();

private:
    void tickNivel1(double dt);
    void tickNivel2(double dt);
    bool estaPresionada(int codigo) const;

    QTimer temporizador;
    QElapsedTimer reloj;
    QSet<int> teclas;
    std::unique_ptr<Nivel1Banco> n1;
    std::unique_ptr<Nivel2Fuga> n2;
    Nivel* actual = nullptr;
    int nivelEnCurso = 0;
    bool modoDificil = false;
    bool caminando = false;
    bool miraIzquierda = false;
    bool golpeando = false;
    bool capturaEnCurso = false;
    double tiempoCaptura = 0.0;
    double tiempoMartillo = 0.0;
};
