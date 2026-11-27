#pragma once
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QVector>

#include "ConfigGrafica.h"
#include "GestorJuego.h"
#include "GestorRecursos.h"
#include "SpriteAnimado.h"

class QEvent;
class QKeyEvent;
class QResizeEvent;
class QGraphicsPixmapItem;
class QMediaPlayer;
class QAudioOutput;
class QTimer;

class VentanaJuego : public QMainWindow {
    Q_OBJECT
public:
    explicit VentanaJuego(GestorJuego* gestor, QWidget* padre = nullptr);

signals:
    void teclaPresionada(int codigo);
    void teclaSoltada(int codigo);

protected:
    void keyPressEvent(QKeyEvent* evento) override;
    void keyReleaseEvent(QKeyEvent* evento) override;
    void resizeEvent(QResizeEvent* evento) override;
    bool eventFilter(QObject* objeto, QEvent* evento) override;

private slots:
    void construirNivel(int numero);
    void actualizarVista(double dt);
    void mostrarFinDeNivel(int numero);
    void mostrarDerrota(int numero);
    void mostrarVictoriaFinal();

private:
    void construirNivel1();
    void construirNivel2();
    void actualizarNivel1(double dt);
    void actualizarNivel2(double dt);
    void mostrarMenu();
    void iniciarNivelDesdeMenu(int numero);
    void actualizarBarra(QLabel* etiqueta, int columna, double proporcion);
    void reproducirGolpe(int golpesConsecutivos);
    void reproducirRisa();

    QPointF aEscena(const QPointF& logico) const;
    QPixmap tira(const Animacion& animacion);
    SpriteAnimado* crearSprite(const Animacion& animacion, const QSizeF& tamano,
                               bool pies, double z);
    void usarAnimacion(SpriteAnimado* sprite, const Animacion& animacion);

    GestorJuego* gestor;
    GestorRecursos recursos;
    QGraphicsScene* escena;
    QGraphicsView* vista;
    QLabel* etiquetaHud;
    QLabel* etiquetaObjetivo;
    QProgressBar* barraPrincipal;
    QProgressBar* barraEnergia;
    QWidget* panelHud;
    QLabel* barraVitalidadImagen;
    QLabel* barraSoporteImagen;
    QLabel* barraBovedaImagen;
    QLabel* barraRuidoImagen;
    QVector<QLabel*> corazonesIntegridad;
    bool enMenu = true;
    QMediaPlayer* musicaFondo;
    QMediaPlayer* sonidoRisa;
    QMediaPlayer* sonidoGolpe;
    QAudioOutput* salidaGolpe;
    QTimer* limiteSonidoGolpe;

    double sx = 1.0;
    double sy = 1.0;
    double tiempoAnimacion = 0.0;

    // Nivel 1 (los items los destruye la escena)
    SpriteAnimado* minionItem = nullptr;
    SpriteAnimado* guardiaItem = nullptr;
    SpriteAnimado* camaraItem = nullptr;
    SpriteAnimado* puertaItem = nullptr;
    SpriteAnimado* soporteItem = nullptr;
    SpriteAnimado* iconoItem = nullptr;
    QGraphicsPolygonItem* conoItem = nullptr;
    QGraphicsSimpleTextItem* letreroItem = nullptr;
    QGraphicsPixmapItem* fondoBanco = nullptr;
    bool fondoAlarmaActivado = false;
    QVector<QGraphicsPixmapItem*> laserItems;
    QVector<QGraphicsPixmapItem*> lasersSueloItems;
    QVector<QGraphicsPixmapItem*> escalerasItems;
    QVector<SpriteAnimado*> itemsItems;
    QPointF ultimaPosGuardia;

    // Nivel 2
    QGraphicsPixmapItem* fondoA = nullptr;
    QGraphicsPixmapItem* fondoB = nullptr;
    double desplazamientoFondo = 0.0;
    SpriteAnimado* carroItem = nullptr;
    SpriteAnimado* policiaItem = nullptr;
    QVector<SpriteAnimado*> obstaculosItems;
};
