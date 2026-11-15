#pragma once

#include <QPointF>
#include <QString>

#include "Boveda.h"
#include "CamaraSeguridad.h"
#include "GuardiaCentinela.h"
#include "Minion.h"
#include "Nivel.h"
#include "Ruido.h"

class Nivel1Banco : public Nivel {
public:
    Nivel1Banco();

    void iniciar() override;
    void actualizar(double deltaTiempo) override;

    void moverMinion(const QPointF& direccion, double deltaTiempo);
    void danarPuerta(double dano);
    void recogerBotin(int cantidad);
    void usarBanana();

    const Minion& obtenerMinion() const;
    const Boveda& obtenerBoveda() const;
    const GuardiaCentinela& obtenerGuardia() const;
    int obtenerBotinRecolectado() const;
    QString obtenerZonaActual() const;

private:
    void procesarRuido(const Ruido& ruido);
    void verificarCaptura();
    void verificarObjetivo();
    QString determinarZona(const QPointF& posicion) const;

    Minion minion;
    Boveda boveda;
    CamaraSeguridad camara;
    GuardiaCentinela guardia;
    int botinRecolectado;
    double radioCaptura;
};
