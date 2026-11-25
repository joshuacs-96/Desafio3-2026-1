#include "GestorRecursos.h"

#include <QDebug>
#include <QPainter>
#include <QVector>
#include <algorithm>

QString GestorRecursos::clave(const QString& base, int cuadros, bool primeroSinNumero) {
    return base + "#" + QString::number(cuadros) + (primeroSinNumero ? "s" : "n");
}

QPixmap GestorRecursos::cargarSprite(const QString& nombre) {
    auto it = cache.constFind(nombre);
    if (it != cache.constEnd()) {
        return it.value();
    }

    QPixmap pixmap(":/" + nombre + ".png");
    if (pixmap.isNull()) {
        throw ExcepcionRecurso("No se pudo cargar el recurso: " + nombre);
    }

    cache.insert(nombre, pixmap);
    return pixmap;
}

QPixmap GestorRecursos::cargarAnimacion(const QString& base, int cuadros, bool primeroSinNumero) {
    const QString llave = clave(base, cuadros, primeroSinNumero);
    auto it = cache.constFind(llave);
    if (it != cache.constEnd()) {
        return it.value();
    }

    QVector<QPixmap> cargados;
    int anchoMax = 0;
    int altoMax = 0;

    for (int i = 0; i < cuadros; ++i) {
        QString nombre = base;
        if (cuadros > 1) {
            const int numero = primeroSinNumero ? i : i + 1;   // martillo, martillo1, martillo2...
            if (numero > 0) {
                nombre += QString::number(numero);
            }
        }

        const QPixmap cuadro = cargarSprite(nombre);           // lanza ExcepcionRecurso si falta
        cargados.append(cuadro);
        anchoMax = std::max(anchoMax, cuadro.width());
        altoMax = std::max(altoMax, cuadro.height());
    }

    // Tira horizontal; cada cuadro centrado en x y pegado abajo
    QPixmap tira(anchoMax * cuadros, altoMax);
    tira.fill(Qt::transparent);

    QPainter pintor(&tira);
    for (int i = 0; i < cuadros; ++i) {
        const QPixmap& cuadro = cargados[i];
        pintor.drawPixmap(i * anchoMax + (anchoMax - cuadro.width()) / 2,
                          altoMax - cuadro.height(), cuadro);
    }
    pintor.end();

    cache.insert(llave, tira);
    return tira;
}

QPixmap GestorRecursos::animacionOPlaceholder(const QString& base, int cuadros,
                                              bool primeroSinNumero, const QSize& cuadro,
                                              const QColor& color) {
    try {
        return cargarAnimacion(base, cuadros, primeroSinNumero);
    } catch (const ExcepcionRecurso& error) {
        qWarning() << error.what();

        QPixmap relleno(cuadro.width() * cuadros, cuadro.height());
        relleno.fill(color);

        QPainter pintor(&relleno);
        pintor.setPen(Qt::black);
        for (int i = 0; i < cuadros; ++i) {
            const QRect zona(i * cuadro.width(), 0, cuadro.width(), cuadro.height());
            pintor.drawRect(zona.adjusted(0, 0, -1, -1));
            pintor.drawText(zona, Qt::AlignCenter | Qt::TextWordWrap, base.section('/', -1));
        }
        pintor.end();

        cache.insert(clave(base, cuadros, primeroSinNumero), relleno);
        return relleno;
    }
}