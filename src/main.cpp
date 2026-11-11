#include <QCoreApplication>
#include <QDebug>

#include "logica/CarroFuga.h"
#include "logica/MovimientoOscilatorio.h"
#include "logica/Personaje.h"

int main(int argc, char *argv[]) {
    QCoreApplication aplicacion(argc, argv);

    Personaje kevin(QPointF(20.0, 20.0));
    kevin.comerBanana();

    qDebug() << "Daño de puerta en un segundo:" << kevin.obtenerDanioPuerta(1.0);

    CarroFuga carro(QPointF(100.0, 200.0), 100);
    carro.acelerar(80.0);
    carro.actualizar(1.0);

    qDebug() << "Posición del carro:" << carro.obtenerPosicion();

    MovimientoOscilatorio camara(50.0, 1.0, 0.0);
    qDebug() << "Desplazamiento de cámara:" << camara.calcular(1.0);

    return 0;
}
