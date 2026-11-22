#include "GuardiaCentinela.h"

#include <QLineF>
#include <QRandomGenerator>
#include <QtGlobal>

#include "Edificio.h"

GuardiaCentinela::GuardiaCentinela(const QPointF& posicionInicial,
                                   double radioPercepcion)
    : posicion(posicionInicial),
    radioPercepcion(radioPercepcion),
    estadoActual("Patrullar"),
    ultimaPosicionRuido(posicionInicial),
    tasaAprendizaje(0.25),
    hayRuidoPendiente(false),
    velocidad(90.0),
    ultimaPosicionVista(posicionInicial) {
}

void GuardiaCentinela::establecerEdificio(const Edificio* nuevoEdificio) {
    edificio = nuevoEdificio;
}

bool GuardiaCentinela::percibir(const QPointF& posicionRuido, double intensidad) {
    QLineF distancia(posicion, posicionRuido);

    if (distancia.length() > radioPercepcion || intensidad <= 0.0) {
        return false;
    }

    ultimaPosicionRuido = posicionRuido;
    hayRuidoPendiente = true;
    ruta.clear();                       // nuevo objetivo: se vuelve a planificar

    return true;
}

void GuardiaCentinela::ver(const QPointF& posicionMinion) {
    viendoMinion = false;

    if (!edificio) {
        return;
    }

    const bool mismoPiso = edificio->pisoDe(posicion) == edificio->pisoDe(posicionMinion);

    if (mismoPiso && QLineF(posicion, posicionMinion).length() <= radioVision) {
        viendoMinion = true;
        ultimaPosicionVista = posicionMinion;
    }
}

void GuardiaCentinela::decidir() {
    if (viendoMinion) {
        estadoActual = "Perseguir";
        return;
    }

    if (estadoActual == "Perseguir") {          // lo perdió de vista: va a donde lo vio
        ultimaPosicionRuido = ultimaPosicionVista;
        hayRuidoPendiente = true;
        ruta.clear();
    }

    if (hayRuidoPendiente) {
        estadoActual = "Investigar";
    }
}

void GuardiaCentinela::actuar(double deltaTiempo) {
    if (estadoActual == "Perseguir") {
        planificar(ultimaPosicionVista);            // Kevin se mueve: se recalcula
        avanzarPorRuta(deltaTiempo, velocidad * 1.4);
        return;
    }

    if (estadoActual == "Investigar") {
        if (ruta.isEmpty()) {
            planificar(ultimaPosicionRuido);
        }

        avanzarPorRuta(deltaTiempo, velocidad);

        if (ruta.isEmpty()) {                       // llegó a donde sonó
            hayRuidoPendiente = false;
            estadoActual = "Patrullar";
            espera = 1.0;
        }
        return;
    }

    // Patrullar
    if (espera > 0.0) {
        espera -= deltaTiempo;
        return;
    }

    if (ruta.isEmpty()) {
        const QString zona = elegirZonaDePatrulla();
        if (zona.isEmpty()) {
            return;
        }
        planificar(posicionesZonas.value(zona));
    }

    avanzarPorRuta(deltaTiempo, velocidad * 0.65);

    if (ruta.isEmpty()) {
        espera = 1.5;                               // se detiene un momento en cada punto
    }
}

void GuardiaCentinela::planificar(const QPointF& destino) {
    if (edificio) {
        ruta = edificio->planificarRuta(posicion, destino);
    } else {
        ruta = QVector<QPointF>{destino};
    }
}

void GuardiaCentinela::avanzarPorRuta(double deltaTiempo, double velocidadActual) {
    double restante = velocidadActual * deltaTiempo;

    while (!ruta.isEmpty() && restante > 0.0) {
        const QPointF objetivo = ruta.first();
        QLineF tramo(posicion, objetivo);

        if (tramo.length() <= restante) {
            posicion = objetivo;
            restante -= tramo.length();
            ruta.removeFirst();
        } else {
            tramo.setLength(restante);
            posicion = tramo.p2();
            restante = 0.0;
        }
    }
}

QString GuardiaCentinela::elegirZonaDePatrulla() const {
    if (posicionesZonas.isEmpty()) {
        return QString();
    }

    // Cada zona pesa 1 + 4*sospecha: patrulla más donde suele encontrar a Kevin
    double total = 0.0;
    for (auto it = posicionesZonas.constBegin(); it != posicionesZonas.constEnd(); ++it) {
        total += 1.0 + 4.0 * consultarZona(it.key());
    }

    double azar = QRandomGenerator::global()->generateDouble() * total;
    for (auto it = posicionesZonas.constBegin(); it != posicionesZonas.constEnd(); ++it) {
        azar -= 1.0 + 4.0 * consultarZona(it.key());
        if (azar <= 0.0) {
            return it.key();
        }
    }

    return posicionesZonas.constBegin().key();
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

void GuardiaCentinela::reiniciar(const QPointF& nuevaPosicion) {
    posicion = nuevaPosicion;
    estadoActual = "Patrullar";
    ruta.clear();
    hayRuidoPendiente = false;
    viendoMinion = false;
    espera = 2.0;                       // margen para que Kevin no sea atrapado dos veces seguidas
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