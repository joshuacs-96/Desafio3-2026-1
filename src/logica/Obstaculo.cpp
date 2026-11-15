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
      factorVelocidad(factorVelocidad) {
}

void Obstaculo::aplicarEfecto(Minion& minion) const {
    minion.recibirDanio(dano);
}

void Obstaculo::aplicarEfecto(CarroJugador& carro) const {
    carro.recibirDanio(dano);

    if (tipo == "Aceite") {
        carro.activarDerrape(factorVelocidad);
    }
}

QString Obstaculo::obtenerTipo() const {
    return tipo;
}

QPointF Obstaculo::obtenerPosicion() const {
    return posicion;
}
