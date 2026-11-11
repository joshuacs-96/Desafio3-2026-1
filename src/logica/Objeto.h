#pragma once

#include "Entidad.h"
#include "TiposJuego.h"

class Objeto : public Entidad {
public:
    Objeto(TipoObjeto tipo, const QPointF& posicionInicial, int valor, int dano);

    void actualizar(double deltaTiempo) override;

    TipoObjeto obtenerTipo() const;
    int obtenerValor() const;
    int obtenerDano() const;

private:
    TipoObjeto tipo;
    int valor;
    int dano;
};
