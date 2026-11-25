#pragma once

#include <QPointF>
#include <QString>
#include <QVector>

#include "Boveda.h"
#include "CamaraSeguridad.h"
#include "Edificio.h"
#include "GuardiaCentinela.h"
#include "ItemRecolectable.h"
#include "Minion.h"
#include "Nivel.h"
#include "Ruido.h"

class Nivel1Banco : public Nivel {
public:
    Nivel1Banco();

    void iniciar() override;
    void actualizar(double deltaTiempo) override;

    void moverMinion(const QPointF& direccion, double deltaTiempo);
    void saltarMinion();
    bool usarEscalera(bool subir);
    void danarPuerta(double dano);

    const Minion& obtenerMinion() const;
    const Boveda& obtenerBoveda() const;
    const CamaraSeguridad& obtenerCamara() const;
    const GuardiaCentinela& obtenerGuardia() const;
    const QVector<ItemRecolectable>& obtenerItems() const;
    const Edificio& obtenerEdificio() const;
    int obtenerBotinRecolectado() const;
    QString obtenerZonaActual() const;
    QPointF obtenerPosicionPuerta() const;
    QPointF obtenerPosicionSalida() const;
    bool puedeEscapar() const;           // puerta abierta y botín mínimo reunido
    void finalizarCaptura();

private:
private:
    void procesarRuido(const Ruido& ruido);
    void recogerItemsCercanos();
    void verificarCaptura();
    void verificarObjetivo();
    void verificarLaser();
    QString determinarZona(const QPointF& posicion) const;

    Minion minion;
    Boveda boveda;
    CamaraSeguridad camara;
    GuardiaCentinela guardia;
    int botinRecolectado;
    double radioCaptura;
    bool laserActivado;
    Edificio edificio;
    QVector<ItemRecolectable> items;
};