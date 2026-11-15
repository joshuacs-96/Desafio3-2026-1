#include <QCoreApplication>
#include <QDebug>

#include "logica/Nivel1Banco.h"
#include "logica/Nivel2Fuga.h"

int main(int argc, char *argv[]) {
    QCoreApplication aplicacion(argc, argv);

    Nivel1Banco nivelBanco;
    nivelBanco.usarBanana();
    nivelBanco.moverMinion(QPointF(1.0, 0.0), 1.0);
    nivelBanco.danarPuerta(100.0);
    nivelBanco.recogerBotin(3);
    nivelBanco.actualizar(0.1);

    qDebug() << "Bóveda abierta:" << nivelBanco.obtenerBoveda().estaAbierta();
    qDebug() << "Botín recolectado:" << nivelBanco.obtenerBotinRecolectado();
    qDebug() << "Nivel completado:" << nivelBanco.estaCompletado();

    Nivel2Fuga nivelFuga(true);
    nivelFuga.acelerarCarro(80.0);
    nivelFuga.actualizar(1.0);

    qDebug() << "Nivel de fuga difícil:" << nivelFuga.estaEnModoDificil();
    qDebug() << "Tiempo restante:" << nivelFuga.obtenerTiempoRestante();

    return 0;
}
