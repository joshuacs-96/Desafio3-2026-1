#include "Boveda.h"

#include <algorithm>

Boveda::Boveda(double resistenciaInicial, int botinMinimo)
    : resistencia(resistenciaInicial),
    abierta(false),
    botinMinimo(botinMinimo),
    resistenciaMaxima(resistenciaInicial){
}

void Boveda::recibirDanio(double dano) {
    if (abierta || dano <= 0.0) {
        return;
    }

    resistencia = std::max(0.0, resistencia - dano);
    abierta = resistencia == 0.0;
}

bool Boveda::estaAbierta() const {
    return abierta;
}

double Boveda::obtenerResistencia() const {
    return resistencia;
}

int Boveda::obtenerBotinMinimo() const {
    return botinMinimo;
}

double Boveda::obtenerResistenciaMaxima() const {
    return resistenciaMaxima;
}
