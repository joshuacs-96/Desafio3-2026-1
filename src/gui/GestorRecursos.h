#pragma once
#include <QColor>
#include <QHash>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <stdexcept>

class ExcepcionRecurso : public std::runtime_error {
public:
    explicit ExcepcionRecurso(const QString& mensaje)
        : std::runtime_error(mensaje.toStdString()) {}
};

class GestorRecursos {
public:
    // Un solo archivo: nombre sin extensión, por ejemplo "Sprits d3/objetos_N1/banana"
    QPixmap cargarSprite(const QString& nombre);

    // Varios cuadros sueltos (base1, base2, ...) unidos en una tira horizontal
    QPixmap cargarAnimacion(const QString& base, int cuadros, bool primeroSinNumero);

    // Igual que cargarAnimacion, pero si falta algún PNG devuelve un rectángulo de color
    QPixmap animacionOPlaceholder(const QString& base, int cuadros, bool primeroSinNumero,
                                  const QSize& cuadro, const QColor& color);

private:
    static QString clave(const QString& base, int cuadros, bool primeroSinNumero);

    QHash<QString, QPixmap> cache;
};