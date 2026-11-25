#pragma once

class Boveda {
public:
    Boveda(double resistenciaInicial, int botinMinimo);

    void recibirDanio(double dano);
    bool estaAbierta() const;
    double obtenerResistencia() const;
    double obtenerResistenciaMaxima() const;
    int obtenerBotinMinimo() const;

private:
    double resistencia;
    bool abierta;
    int botinMinimo;
    double resistenciaMaxima;
};
