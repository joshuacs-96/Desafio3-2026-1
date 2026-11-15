#include "GuardiaCentinela.h"

#include <QLineF>

GuardiaCentinela::GuardiaCentinela(const QPointF& posicionInicial,
                                   double radioPercepcion)
    : posicion(posicionInicial),
      radioPercepcion(radioPercepcion),
      estadoActual("Patrullar"),
      ultimaPosicionRuido(posicionInicial),
      tasaAprendizaje(0.25),
      umbralInvestigacion(0.30),
      hayRuidoPendiente(false),
      indiceRutaActual(0),
      velocidad(90.0) {
}

bool GuardiaCentinela::percibir(const QPointF& posicionRuido, double intensidad) {
    QLineF distancia(posicion, posicionRuido);

    if (distancia.length() > radioPercepcion || intensidad <= 0.0) {
        return false;
    }

    ultimaPosicionRuido = posicionRuido;
    hayRuidoPendiente = true;

    return true;
}

void GuardiaCentinela::decidir() {
    if (hayRuidoPendiente) {
        estadoActual = "Investigar";
        return;
    }

    QString zonaSospechosa = obtenerZonaMasSospechosa();

    if (!zonaSospechosa.isEmpty()
        && consultarZona(zonaSospechosa) >= umbralInvestigacion) {
        ultimaPosicionRuido = posicionesZonas.value(zonaSospechosa);
        estadoActual = "Investigar";
    } else {
        estadoActual = "Patrullar";
    }
}

void GuardiaCentinela::actuar(double deltaTiempo) {
    if (estadoActual != "Investigar") {
        return;
    }

    QLineF camino(posicion, ultimaPosicionRuido);

    if (camino.length() <= velocidad * deltaTiempo) {
        posicion = ultimaPosicionRuido;
        hayRuidoPendiente = false;
        estadoActual = "Patrullar";
        return;
    }

    camino.setLength(velocidad * deltaTiempo);
    posicion = camino.p2();
}

void GuardiaCentinela::registrarZona(const QString& zona,
                                     const QPointF& posicionZona) {
    posicionesZonas[zona] = posicionZona;

    if (!mapaCalor.contains(zona)) {
        mapaCalor[zona] = 0.0;
    }
}

void GuardiaCentinela::aprender(const QString& zona, bool encontroMinion) {
    double recompensa = encontroMinion ? 1.0 : 0.0;
    double valorActual = mapaCalor.value(zona, 0.0);

    mapaCalor[zona] = valorActual + tasaAprendizaje * (recompensa - valorActual);
}

QString GuardiaCentinela::obtenerEstadoActual() const {
    return estadoActual;
}

QPointF GuardiaCentinela::obtenerPosicion() const {
    return posicion;
}

double GuardiaCentinela::consultarZona(const QString& zona) const {
    return mapaCalor.value(zona, 0.0);
}

QString GuardiaCentinela::obtenerZonaMasSospechosa() const {
    QString zonaMasSospechosa;
    double valorMasAlto = 0.0;

    for (auto iterador = mapaCalor.constBegin();
         iterador != mapaCalor.constEnd();
         ++iterador) {
        if (iterador.value() > valorMasAlto) {
            valorMasAlto = iterador.value();
            zonaMasSospechosa = iterador.key();
        }
    }

    return zonaMasSospechosa;
}
