#pragma once

#include <QPointF>

enum class TipoObjeto {
    Banana,
    Botin,
    Cono,
    Barrera,
    Hueco,
    CharcoAceite
};

enum class TipoRuido {
    Pasos,
    Puerta,
    Risa,
    Impacto
};

enum class TipoDificultad {
    Facil,
    Dificil
};

struct Zona {
    int fila;
    int columna;

    bool operator<(const Zona& otra) const;
};

struct Ruido {
    TipoRuido tipo;
    QPointF posicion;
    double intensidad;
    double duracion;
};
