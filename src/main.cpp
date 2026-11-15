#include <QCoreApplication>
#include <QDebug>

#include "logica/CarroJugador.h"
#include "logica/Minion.h"
#include "logica/Obstaculo.h"

int main(int argc, char *argv[]) {
    QCoreApplication aplicacion(argc, argv);

    Minion kevin;
    kevin.comerBanana();
    kevin.actualizar(3.0);

    if (kevin.hayRisaPendiente()) {
        Ruido risa = kevin.generarRisaAutomatica();
        qDebug() << "El Minion genera ruido:" << risa.tipo;
    }

    CarroJugador carro;
    carro.acelerar(80.0);
    carro.actualizar(1.0);

    qDebug() << "Posición del carro:" << carro.obtenerPosicion();

    Obstaculo aceite("Aceite", QPointF(0.0, 0.0), 0, 2.0);
    aceite.aplicarEfecto(carro);

    qDebug() << "El carro derrapa:" << carro.estaDerrapando();

    return 0;
}
