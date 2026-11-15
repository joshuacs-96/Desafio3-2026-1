#pragma once

class Nivel {
public:
    virtual ~Nivel() = default;

    virtual void iniciar() = 0;
    virtual void actualizar(double deltaTiempo) = 0;

    bool estaCompletado() const;
    bool estaPerdido() const;

protected:
    bool completado = false;
    bool perdido = false;
};
