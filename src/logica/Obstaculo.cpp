#include "Obstaculo.h"

#include "CarroJugador.h"
#include "Minion.h"

Obstaculo::Obstaculo(const QString& tipo,
                     const QPointF& posicion,
                     int dano,
                     double factorVelocidad)
    : posicion(posicion),
      tipo(tipo),
      dano(dano),
      factorVelocidad(factorVelocidad),
      radioColision(28.0),
      activo(true) {
}

void Obstaculo::aplicarEfecto(Minion& minion) {
    minion.recibirDanio(dano);
}

void Obstaculo::aplicarEfecto(CarroJugador& carro) {
    if (!activo) {
        return;
    }

    if (carro.estaProtegido()) {
        activo = false;
        return;
    }
    carro.recibirDanio(dano);

    if (tipo == "Aceite") {
        carro.activarDerrape(factorVelocidad);
        activo = false;
        return;
    }
    if (tipo == "Barrera") {
        carro.rebotar();
        carro.perderBotin(1);
    }

    activo = false;
}

void Obstaculo::moverVerticalmente(double desplazamiento) {
    posicion.ry() += desplazamiento;
}

void Obstaculo::moverHorizontalmente(double desplazamiento) {
    posicion.rx() += desplazamiento;
}

QString Obstaculo::obtenerTipo() const {
    return tipo;
}

QPointF Obstaculo::obtenerPosicion() const {
    return posicion;
}

double Obstaculo::obtenerRadioColision() const {
    return radioColision;
}

bool Obstaculo::estaActivo() const {
    return activo;
}
