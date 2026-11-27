#include "VentanaJuego.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineF>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QtMath>
#include <cmath>
#include <QTransform>
#include <QColor>
#include <QBrush>
#include <QPen>

namespace {
// Cada tipo de obstáculo de la lógica tiene su dibujo
const Animacion& animacionDeObstaculo(const QString& tipo) {
    using namespace ConfigGrafica;
    if (tipo == "Aceite") {
        return Anim::OBS_ACEITE;
    }
    if (tipo == "Hueco") {
        return Anim::OBS_HUECO;
    }
    if (tipo == "Barrera") {
        return Anim::OBS_BARRERA;
    }
    return Anim::OBS_CONO;
}
}

VentanaJuego::VentanaJuego(GestorJuego* gestor, QWidget* padre)
    : QMainWindow(padre), gestor(gestor) {
    setWindowTitle("Minions y el Robo del Siglo");
    setFocusPolicy(Qt::StrongFocus);

    escena = new QGraphicsScene(this);
    vista = new QGraphicsView(escena);
    vista->setRenderHint(QPainter::SmoothPixmapTransform, false);   // pixel art nítido
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFocusPolicy(Qt::NoFocus);                              // las teclas las recibe la ventana
    vista->setBackgroundBrush(Qt::black);
    vista->viewport()->installEventFilter(this);                     // clic = coordenadas lógicas

    panelHud = new QWidget(vista->viewport());
    panelHud->setStyleSheet("background-color: rgba(0, 0, 0, 190); color: white; font-size: 14px;");
    panelHud->setAttribute(Qt::WA_TransparentForMouseEvents);
    etiquetaHud = new QLabel(panelHud);
    barraEnergia = new QProgressBar(panelHud);
    barraPrincipal = new QProgressBar(panelHud);
    barraEnergia->setRange(0, 100);
    barraPrincipal->setRange(0, 100);

    auto* fila = new QHBoxLayout(panelHud);
    fila->addWidget(etiquetaHud, 1);
    fila->addWidget(barraEnergia);
    fila->addWidget(barraPrincipal);

    auto* central = new QWidget;
    auto* columna = new QVBoxLayout(central);
    columna->addWidget(vista, 1);
    setCentralWidget(central);
    panelHud->setGeometry(10, 10, 900, 65);
    panelHud->raise();

    // teclado: ventana -> gestor
    connect(this, &VentanaJuego::teclaPresionada, gestor, &GestorJuego::teclaPresionada);
    connect(this, &VentanaJuego::teclaSoltada, gestor, &GestorJuego::teclaSoltada);

    // juego: gestor -> ventana
    connect(gestor, &GestorJuego::nivelIniciado, this, &VentanaJuego::construirNivel);
    connect(gestor, &GestorJuego::estadoActualizado, this, &VentanaJuego::actualizarVista);

    // En cola, para que los mensajes no se muestren dentro del tick del temporizador
    connect(gestor, &GestorJuego::nivelCompletado, this, &VentanaJuego::mostrarFinDeNivel,
            Qt::QueuedConnection);
    connect(gestor, &GestorJuego::nivelPerdido, this, &VentanaJuego::mostrarDerrota,
            Qt::QueuedConnection);
    connect(gestor, &GestorJuego::juegoGanado, this, &VentanaJuego::mostrarVictoriaFinal,
            Qt::QueuedConnection);

    resize(960, 700);
}

void VentanaJuego::keyPressEvent(QKeyEvent* evento) {
    if (!evento->isAutoRepeat()) {
        emit teclaPresionada(evento->key());
    }
}

void VentanaJuego::keyReleaseEvent(QKeyEvent* evento) {
    if (!evento->isAutoRepeat()) {
        emit teclaSoltada(evento->key());
    }
}

void VentanaJuego::resizeEvent(QResizeEvent* evento) {
    QMainWindow::resizeEvent(evento);
    vista->fitInView(escena->sceneRect(), Qt::KeepAspectRatio);
    panelHud->setGeometry(10, 10, qMax(1, vista->viewport()->width() - 20), 65);
}

// Un clic sobre el juego imprime las coordenadas LÓGICAS: úsalas para ubicar pisos,
// escaleras, puerta, salida y objetos sobre tu fondo.
bool VentanaJuego::eventFilter(QObject* objeto, QEvent* evento) {
    if (objeto == vista->viewport() && evento->type() == QEvent::MouseButtonPress) {
        auto* clic = static_cast<QMouseEvent*>(evento);
        const QPointF enEscena = vista->mapToScene(clic->pos());
        qDebug() << "Coordenadas lógicas:" << enEscena.x() / sx << enEscena.y() / sy;
    }
    return QMainWindow::eventFilter(objeto, evento);
}

// ---------------------------------------------------------------- utilidades

QPointF VentanaJuego::aEscena(const QPointF& logico) const {
    return QPointF(logico.x() * sx, logico.y() * sy);
}

QPixmap VentanaJuego::tira(const Animacion& animacion) {
    const QString base = QString::fromUtf8(animacion.base);
    const QColor color = QColor::fromHsv(static_cast<int>(qHash(base) % 360), 140, 230);
    return recursos.animacionOPlaceholder(base, animacion.cuadros, animacion.primeroSinNumero,
                                          QSize(animacion.ancho, animacion.alto), color);
}

SpriteAnimado* VentanaJuego::crearSprite(const Animacion& animacion, const QSizeF& tamano,
                                         bool pies, double z) {
    auto* item = new SpriteAnimado(tira(animacion), animacion.cuadros, animacion.fps, tamano, pies);
    item->setZValue(z);
    escena->addItem(item);
    return item;
}

void VentanaJuego::usarAnimacion(SpriteAnimado* sprite, const Animacion& animacion) {
    sprite->cambiarTira(tira(animacion), animacion.cuadros, animacion.fps);
}

// ---------------------------------------------------------------- construcción

void VentanaJuego::construirNivel(int numero) {
    escena->clear();    // destruye todos los items
    escudoItem = nullptr;
    minionItem = guardiaItem = camaraItem = puertaItem = iconoItem = nullptr;
    soporteItem = nullptr;
    policiaItem = carroItem = nullptr;
    conoItem = nullptr;
    letreroItem = nullptr;
    fondoA = fondoB = nullptr;
    fondoBanco = nullptr;
    fondoAlarmaActivado = false;
    laserItems.clear();
    escalerasItems.clear();
    itemsItems.clear();
    obstaculosItems.clear();
    recolectablesN2Items.clear();
    tiempoAnimacion = 0.0;

    if (numero == 1) {
        construirNivel1();
    } else {
        construirNivel2();
    }

    vista->fitInView(escena->sceneRect(), Qt::KeepAspectRatio);
}

void VentanaJuego::construirNivel1() {
    using namespace ConfigGrafica;
    const Nivel1Banco* n = gestor->nivel1();

    const QPixmap fondo = tira(Anim::FONDO_NIVEL1);
    escena->setSceneRect(fondo.rect());
    auto* fondoItem = escena->addPixmap(fondo);
    fondoBanco = fondoItem;
    fondoItem->setZValue(0);
    fondoItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    sx = fondo.width() / ANCHO_LOGICO_1;
    sy = fondo.height() / ALTO_LOGICO_1;

    // Los tres rayos verticales corresponden a los de la lógica del nivel.
    const QPointF posicionesLaser[] = {
        QPointF(250.0, 106.458),
        QPointF(355.0, 106.458),
        QPointF(350.0, 206.654)
    };
    const QPixmap laser = tira(Anim::RAYO_LASER).scaled(
        static_cast<int>(8 * sx), static_cast<int>(99 * sy),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    for (const QPointF& posicion : posicionesLaser) {
        QGraphicsPixmapItem* item = escena->addPixmap(laser);
        item->setOffset(-laser.width() / 2.0, -laser.height());
        item->setPos(aEscena(posicion + QPointF(0.0, 5.0)));
        item->setZValue(2.5);
        laserItems.append(item);
    }

    // marca TEMPORAL de la salida (bórrala cuando tu fondo ya la dibuje)
    auto* marcaSalida = escena->addRect(-24, -64, 48, 64, QPen(Qt::green, 3));
    marcaSalida->setPos(aEscena(n->obtenerPosicionSalida()));
    marcaSalida->setZValue(1);

    // escaleras: un dibujo por cada tramo (solo las de subida; las de bajada son las mismas)
    if (DIBUJAR_ESCALERAS) {
        const QPixmap dibujoEscalera = tira(Anim::ESCALERA);
        for (const Escalera& escalera : n->obtenerEdificio().escaleras()) {
            if (escalera.obtenerDestino().y() >= escalera.obtenerPosicion().y()) {
                continue;
            }
            const QPointF abajo = aEscena(escalera.obtenerPosicion());
            const QPointF arriba = aEscena(escalera.obtenerDestino());
            const int alto = static_cast<int>(abajo.y() - arriba.y());

            auto* item = escena->addPixmap(dibujoEscalera.scaled(static_cast<int>(ANCHO_ESCALERA), alto,
                                                                 Qt::IgnoreAspectRatio,
                                                                 Qt::FastTransformation));
            item->setOffset(-item->pixmap().width() / 2.0, -item->pixmap().height());
            item->setPos(abajo);
            item->setZValue(0.5);
            escalerasItems.append(item);
        }
    }

    puertaItem = crearSprite(Anim::PUERTA_CERRADA, TAM_PUERTA, true, 1);
    puertaItem->setPos(aEscena(n->obtenerPosicionPuerta()));
    soporteItem = crearSprite(Anim::SOPORTE, QSizeF(220, 220), true, 2);
    soporteItem->setPos(aEscena(n->obtenerPosicionSoporte() + QPointF(0.0, 5.0)));

    // botín y bananas: cada uno se dibuja según su tipo
    for (const ItemRecolectable& item : n->obtenerItems()) {
        const bool esBanana = item.obtenerTipo() == "Banana";
        const bool esLingote = item.obtenerTipo() == "Lingote";
        const Animacion& animacion = esBanana ? Anim::BANANA : (esLingote ? Anim::LINGOTE : Anim::BOLSA);
        const QSizeF& tamano = esBanana ? TAM_BANANA : (esLingote ? TAM_LINGOTE : TAM_BOLSA);
        itemsItems.append(crearSprite(animacion, tamano, true, 2));
    }

    camaraItem = crearSprite(Anim::CAMARA, TAM_CAMARA, false, 2);
    camaraItem->setPos(aEscena(n->obtenerCamara().obtenerPosicion()));

    conoItem = escena->addPolygon(QPolygonF(), QPen(Qt::NoPen), QBrush(QColor(255, 255, 0, 70)));
    conoItem->setZValue(2);
    conoItem->setParentItem(fondoItem);          // el cono se recorta al área del fondo

    guardiaItem = crearSprite(Anim::POLICIA_QUIETO, TAM_GUARDIA, true, 3);
    ultimaPosGuardia = n->obtenerGuardia().obtenerPosicion();

    minionItem = crearSprite(Anim::MINION_IDLE, TAM_MINION, true, 4);

    iconoItem = crearSprite(Anim::SIGNO_INTERROGACION, TAM_ICONO, false, 5);
    iconoItem->setVisible(false);

    // letrero que aparece sobre la puerta
    letreroItem = escena->addSimpleText("");
    QFont fuente = letreroItem->font();
    fuente.setPixelSize(22);
    fuente.setBold(true);
    letreroItem->setFont(fuente);
    letreroItem->setBrush(Qt::white);
    letreroItem->setPen(QPen(Qt::black, 2));
    letreroItem->setZValue(10);
    letreroItem->setVisible(false);

    barraEnergia->setVisible(true);
    barraEnergia->setFormat("Vitalidad %p%");
    barraPrincipal->setFormat("Bóveda %p%");
    statusBar()->showMessage("A / D: mover   |   W: saltar o subir escalera   |   S: bajar escalera   |   "
                             "ESPACIO (mantener): golpear la puerta   |   Las bananas dan fuerza");
}

void VentanaJuego::construirNivel2() {
    using namespace ConfigGrafica;

    const QPixmap fondo = tira(Anim::FONDO_NIVEL2);
    escena->setSceneRect(fondo.rect());
    fondoA = escena->addPixmap(fondo);
    fondoB = escena->addPixmap(fondo);       // segunda copia, a la derecha, para el scroll continuo
    fondoA->setZValue(0);
    fondoB->setZValue(0);
    fondoB->setPos(fondo.width(), 0);
    desplazamientoFondo = 0.0;
    sx = fondo.width() / ANCHO_LOGICO_2;
    sy = fondo.height() / ALTO_LOGICO_2;

    policiaItem = crearSprite(Anim::CARRO_POLICIA, TAM_CARRO, false, 3);
    policiaItem->setRotation(ROTACION_CARROS);
    policiaItem->setVisible(false);          // solo aparece en difícil, cuando le toca pasar

    carroItem = crearSprite(Anim::CARRO_NORMAL, TAM_CARRO, false, 4);
    carroItem->setRotation(ROTACION_CARROS);
    // Escudo visual del carro
    escudoItem = escena->addEllipse(
        -48.0 * sx,
        -48.0 * sy,
        96.0 * sx,
        96.0 * sy,
        QPen(QColor(60, 200, 255, 230), 3.0),
        QBrush(QColor(30, 160, 255, 45))
        );

    escudoItem->setZValue(4);
    escudoItem->setVisible(false);

    // letrero de peligro sobre el carro
    letreroItem = escena->addSimpleText("");
    QFont fuente = letreroItem->font();
    fuente.setPixelSize(22);
    fuente.setBold(true);
    letreroItem->setFont(fuente);
    letreroItem->setBrush(Qt::white);
    letreroItem->setPen(QPen(Qt::black, 2));
    letreroItem->setZValue(10);
    letreroItem->setVisible(false);

    barraEnergia->setVisible(false);
    barraPrincipal->setFormat("Integridad %p%");
    statusBar()->showMessage("W / ↑: subir de carril   |   S / ↓: bajar de carril   |   "
                             "Esquiva los obstáculos y aguanta hasta que se acabe el tiempo");
}

// ---------------------------------------------------------------- actualización

void VentanaJuego::actualizarVista(double dt) {
    if (gestor->numeroNivel() == 1) {
        actualizarNivel1(dt);
    } else {
        actualizarNivel2(dt);
    }
}

void VentanaJuego::actualizarNivel1(double dt) {
    using namespace ConfigGrafica;
    const Nivel1Banco* n = gestor->nivel1();
    if (!n || !minionItem) {
        return;
    }

    tiempoAnimacion += dt;
    const Minion& minion = n->obtenerMinion();
    const Boveda& boveda = n->obtenerBoveda();
    if (boveda.estaAbierta() && fondoBanco && !fondoAlarmaActivado) {
        QPixmap fondoAlarma(":/Sprits d3/escenarios/Escenario_N1_alarma.jpg");
        if (!fondoAlarma.isNull()) {
            fondoBanco->setPixmap(fondoAlarma.scaled(fondoBanco->pixmap().size()));
            fondoAlarmaActivado = true;
            const QPixmap escaleraRoja = tira(Anim::ESCALERA_ROJA);
            for (QGraphicsPixmapItem* escalera : escalerasItems) {
                if (std::abs(escalera->pos().x() / sx - 320.0) < 1.0
                    && std::abs(escalera->pos().y() / sy - 305.597) < 1.0) {
                    escalera->setVisible(false);
                } else {
                    escalera->setPixmap(escaleraRoja.scaled(escalera->pixmap().size(),
                                                             Qt::IgnoreAspectRatio,
                                                             Qt::FastTransformation));
                }
            }
        }
    }
    const bool fuerte = minion.tieneBananaActiva();
    usarAnimacion(soporteItem, boveda.estaAbierta() ? Anim::SOPORTE_DESTRUIDO_ROJO
                      : (n->soporteDestruido() ? Anim::SOPORTE_DESTRUIDO : Anim::SOPORTE));
    soporteItem->fijarTamano(n->soporteDestruido() ? QSizeF(280, 280) : QSizeF(220, 220));
    soporteItem->setPos(aEscena(n->obtenerPosicionSoporte()
                                 + QPointF(0.0, n->soporteDestruido() ? 9.0 : 5.0)));
    if (laserItems.size() == 3) {
        laserItems[0]->setVisible(!n->soporteDestruido());
        laserItems[1]->setVisible(!boveda.estaAbierta());
        laserItems[2]->setVisible(!boveda.estaAbierta());
    }

    // --- Kevin: la animación depende de lo que está haciendo ---
    minionItem->fijarTamano(fuerte ? TAM_MINION_FUERTE : TAM_MINION);

    if (gestor->faseCaptura() == 1) {
        usarAnimacion(minionItem, Anim::MINION_CAPTURADO);

    } else if (gestor->faseCaptura() == 2) {
        usarAnimacion(minionItem, Anim::MINION_GOLPEADO);

    } else if (minion.estaEnEscalera()) {
        usarAnimacion(minionItem, Anim::MINION_ESCALERA);

    } else if (minion.estaSaltando()) {
        usarAnimacion(minionItem, Anim::MINION_SALTAR);

        const double vy = minion.obtenerVelocidadVertical();
        minionItem->fijarCuadro(vy > 120.0 ? 0 : (vy > -120.0 ? 1 : 2));

    } else if (gestor->minionGolpeando()) {
        usarAnimacion(minionItem,
                      fuerte ? Anim::FORTACHON_GOLPEAR : Anim::MINION_GOLPEAR);

    } else if (gestor->minionCaminando()) {
        usarAnimacion(minionItem,
                      fuerte ? Anim::FORTACHON_CAMINAR : Anim::MINION_CAMINAR);

    } else {
        usarAnimacion(minionItem,
                      fuerte ? Anim::FORTACHON_IDLE : Anim::MINION_IDLE);
    }
    minionItem->espejar(gestor->minionMiraIzquierda() == MINION_MIRA_DERECHA);
    minionItem->avanzar(dt);
    minionItem->setPos(aEscena(minion.obtenerPosicion()));

    // --- policía ---
    const GuardiaCentinela& guardia = n->obtenerGuardia();
    const QString estado = guardia.obtenerEstadoActual();
    const bool persiguiendo = estado == "Perseguir";
    const bool investigando = estado == "Investigar";
    const bool alerta = estado == "Alerta";
    const QPointF posGuardia = guardia.obtenerPosicion();
    const bool seMueve = posGuardia != ultimaPosGuardia;
    const bool atrapando = gestor->faseCaptura() != 0
                           || (persiguiendo
                               && QLineF(posGuardia,
                                         minion.obtenerPosicionSuelo()).length() < 45.0);

    if (atrapando) {
        usarAnimacion(guardiaItem, Anim::POLICIA_ATRAPANDO);
    } else if (seMueve) {
        usarAnimacion(guardiaItem, (persiguiendo || investigando || alerta) ? Anim::POLICIA_CORRER
                                                                  : Anim::POLICIA_CAMINAR);
    } else {
        usarAnimacion(guardiaItem, Anim::POLICIA_QUIETO);
    }

    if (seMueve && posGuardia.x() != ultimaPosGuardia.x()) {
        guardiaItem->espejar((posGuardia.x() < ultimaPosGuardia.x()) == GUARDIA_MIRA_DERECHA);
    }
    guardiaItem->avanzar(dt);
    ultimaPosGuardia = posGuardia;
    guardiaItem->setPos(aEscena(posGuardia));

    iconoItem->setVisible(persiguiendo || investigando || alerta);
    if (persiguiendo || alerta) {
        usarAnimacion(iconoItem, Anim::SIGNO_ADMIRACION);
    } else if (investigando) {
        usarAnimacion(iconoItem, Anim::SIGNO_INTERROGACION);
    }
    iconoItem->setPos(guardiaItem->pos() + QPointF(0, -TAM_GUARDIA.height() - 10));

    // --- cámara: el cono se dibuja con el ángulo que calcula el movimiento oscilatorio ---
    const CamaraSeguridad& camara = n->obtenerCamara();
    const double centro = camara.obtenerAnguloActual();
    const double mitad = camara.obtenerAperturaCono() / 2.0;

    QPolygonF cono;
    cono << aEscena(camara.obtenerPosicion());
    for (int i = 0; i <= 12; ++i) {
        const double angulo = qDegreesToRadians(centro - mitad + 2.0 * mitad * i / 12.0);
        // QLineF mide el ángulo hacia arriba; en pantalla la y crece hacia abajo, por eso -sin
        const QPointF punta = camara.obtenerPosicion()
                              + QPointF(std::cos(angulo), -std::sin(angulo)) * camara.obtenerAlcance();
        cono << aEscena(punta);
    }
    QPolygonF limitePiso;
    limitePiso << aEscena(QPointF(0.0, 0.0))
               << aEscena(QPointF(400.0, 0.0))
               << aEscena(QPointF(400.0, 106.458))
               << aEscena(QPointF(0.0, 106.458));
    conoItem->setPolygon(cono.intersected(limitePiso));
    conoItem->setBrush((boveda.estaAbierta() || camara.detecta(minion))
                           ? QColor(255, 40, 40, 90) : QColor(255, 255, 0, 70));

    // --- puerta: cerrada, dañada o abierta ---
    const double proporcion = boveda.obtenerResistencia() / boveda.obtenerResistenciaMaxima();
    usarAnimacion(puertaItem, boveda.estaAbierta() ? Anim::PUERTA_ABIERTA
                                                   : (proporcion < 0.7 ? Anim::PUERTA_DANADA
                                                                       : Anim::PUERTA_CERRADA));

    // --- botín y bananas: se ven si no fueron recogidos y (si son de la bóveda) la puerta está abierta ---
    const QVector<ItemRecolectable>& items = n->obtenerItems();
    for (int i = 0; i < items.size() && i < itemsItems.size(); ++i) {
        const ItemRecolectable& item = items[i];
        const bool visible = !item.fueRecogido() && (!item.estaDentroDeBoveda() || boveda.estaAbierta());
        itemsItems[i]->setVisible(visible);
        const double flotar = std::sin(tiempoAnimacion * 4.0 + i) * 3.0;     // pequeño vaivén
        itemsItems[i]->setPos(aEscena(item.obtenerPosicion()) + QPointF(0.0, flotar));
    }

    // --- letrero sobre la puerta ---
    const bool avisarPuerta = gestor->cercaDeLaPuerta() && !boveda.estaAbierta();
    letreroItem->setVisible(avisarPuerta);
    if (avisarPuerta) {
        letreroItem->setText("Mantén ESPACIO");
        const QPointF base = aEscena(n->obtenerPosicionPuerta());
        letreroItem->setPos(base.x() - letreroItem->boundingRect().width() / 2.0,
                            base.y() - TAM_PUERTA.height() - 36.0);
    }

    // --- HUD: siempre dice qué hacer ---
    QString objetivo;
    if (!n->soporteDestruido()) {
        objetivo = "Sube al piso 2 y destruye el soporte del láser";
    } else if (!boveda.estaAbierta()) {
        objetivo = "Sube al piso 3 y mantén ESPACIO junto a la puerta de la bóveda";
    } else if (!n->puedeEscapar()) {
        objetivo = "¡Entra a la bóveda y recoge el botín (oro y bolsas)!";
    } else {
        objetivo = "¡Corre a la salida!";
    }

    int botinTotal = 0;
    for (const ItemRecolectable& item : n->obtenerItems()) {
        if (item.obtenerTipo() != "Banana") botinTotal += item.obtenerValor();
    }
    etiquetaHud->setText(QString("Nivel 1  |  Vidas: %1  |  Botín: %2/%3  |  Policía: %4  |  Ruido: %5\n%6")
                             .arg(minion.obtenerVidas())
                             .arg(n->obtenerBotinRecolectado())
                             .arg(botinTotal)
                             .arg(estado)
                             .arg(n->obtenerRuidoGolpes())
                             .arg(objetivo));
    barraEnergia->setValue(minion.obtenerEnergia());
    barraPrincipal->setValue(static_cast<int>(100.0 * proporcion));
}

void VentanaJuego::actualizarNivel2(double dt) {
    using namespace ConfigGrafica;
    const Nivel2Fuga* n = gestor->nivel2();
    if (!n || !carroItem) {
        return;
    }

    tiempoAnimacion += dt;

    // --- carretera: se mueve hacia la izquierda (el carro "avanza" hacia la derecha) ---
    if (SCROLL_FONDO_NIVEL2) {
        const double ancho = fondoA->pixmap().width();
        desplazamientoFondo = std::fmod(desplazamientoFondo + n->obtenerVelocidadScroll() * sx * dt, ancho);
        fondoA->setPos(-desplazamientoFondo, 0);
        fondoB->setPos(ancho - desplazamientoFondo, 0);
    }

    // --- carro: el dibujo depende de la integridad (normal, semidañado, dañado) ---
    const CarroJugador& carro = n->obtenerCarroJugador();
    const double integridad = carro.obtenerIntegridad();
    usarAnimacion(carroItem, integridad > 66.0 ? Anim::CARRO_NORMAL
                                               : (integridad > 33.0 ? Anim::CARRO_SEMIDANADO
                                                                    : Anim::CARRO_DANADO));
    carroItem->setPos(aEscena(carro.obtenerPosicion()));
    carroItem->setRotation(ROTACION_CARROS + (carro.estaDerrapando() ? 18.0 : 0.0));   // se ladea al derrapar

    // Mostrar el escudo mientras dure la protección
    const bool escudoActivo = carro.estaProtegido();

    escudoItem->setVisible(escudoActivo);

    if (escudoActivo) {
        escudoItem->setPos(aEscena(carro.obtenerPosicion()));
    }

    // --- patrulla (solo existe en difícil) ---
    const CarroPolicia& policia = n->obtenerCarroPolicia();
    policiaItem->setVisible(policia.estaEnPantalla());
    policiaItem->setPos(aEscena(policia.obtenerPosicion()));

    // --- obstáculos que aparecen y desaparecen ---
    sincronizarObstaculos(n->obtenerObstaculos());
    sincronizarRecolectables(n->obtenerRecolectables());

    // --- letrero de peligro ---
    const bool peligro = policia.estaEnPantalla() && policia.estaCercaDelJugador();
    letreroItem->setVisible(peligro);
    if (peligro) {
        letreroItem->setText("¡Patrulla cerca!");
        const QPointF base = aEscena(carro.obtenerPosicion());
        letreroItem->setPos(base.x() - letreroItem->boundingRect().width() / 2.0, base.y() - 70.0);
    }

    // --- HUD ---
    QString consejo = n->estaEnModoDificil()
                          ? "Esquiva los obstáculos y cuidado con la patrulla"
                          : "Esquiva los obstáculos hasta que se acabe el tiempo";

    QString estadoEscudo;

    if (carro.estaProtegido()) {
        estadoEscudo = QString(" | ESCUDO: %1 s")
        .arg(carro.obtenerTiempoEscudo(), 0, 'f', 1);
    }

    etiquetaHud->setText(
        QString("Nivel 2 (%1) | Tiempo: %2 s | Botín: %3 | Integridad: %4%5\n%6")
            .arg(n->estaEnModoDificil() ? "Difícil" : "Fácil")
            .arg(qCeil(n->obtenerTiempoRestante()))
            .arg(carro.obtenerBotinActual())
            .arg(static_cast<int>(integridad))
            .arg(estadoEscudo)
            .arg(consejo));

    barraPrincipal->setValue(static_cast<int>(integridad));

    // Sincronizar los obstáculos con sus sprites
    sincronizarObstaculos(n->obtenerObstaculos());

    // Sincronizar los objetos recolectables
    sincronizarRecolectables(n->obtenerRecolectables());
}

void VentanaJuego::sincronizarObstaculos(
    const QVector<Obstaculo>& obstaculos) {

    using namespace ConfigGrafica;

    // Eliminar sprites de obstáculos que salieron de pantalla
    while (obstaculosItems.size() > obstaculos.size()) {
        delete obstaculosItems.takeFirst();
    }

    // Crear sprites para los obstáculos nuevos
    for (int i = obstaculosItems.size(); i < obstaculos.size(); ++i) {
        obstaculosItems.append(
            crearSprite(
                animacionDeObstaculo(obstaculos[i].obtenerTipo()),
                TAM_OBSTACULO,
                false,
                2
                )
            );
    }

    // Actualizar posición y visibilidad
    for (int i = 0; i < obstaculos.size(); ++i) {
        obstaculosItems[i]->setVisible(obstaculos[i].estaActivo());

        obstaculosItems[i]->setPos(
            aEscena(obstaculos[i].obtenerPosicion())
            );
    }
}

void VentanaJuego::sincronizarRecolectables(
    const QVector<RecolectableNivel2>& items) {

    using namespace ConfigGrafica;

    // Eliminar sprites de objetos que ya salieron de la pantalla
    while (recolectablesN2Items.size() > items.size()) {
        delete recolectablesN2Items.takeFirst();
    }

    // Crear sprites para los objetos nuevos
    for (int i = recolectablesN2Items.size(); i < items.size(); ++i) {
        if (items[i].tipo == "Banana") {
            recolectablesN2Items.append(
                crearSprite(Anim::BANANA_N2, Anim::TAM_BANANA_N2, false, 2.5));
        } else {
            recolectablesN2Items.append(
                crearSprite(Anim::LINGOTE_N2, Anim::TAM_LINGOTE_N2, false, 2.5));
        }
    }

    // Actualizar posición y visibilidad
    for (int i = 0; i < items.size(); ++i) {
        recolectablesN2Items[i]->setVisible(items[i].activo);

        const double flotar = std::sin(tiempoAnimacion * 4.0 + i) * 3.0;

        recolectablesN2Items[i]->setPos(
            aEscena(items[i].posicion) + QPointF(0.0, flotar));
    }
}

// ---------------------------------------------------------------- fin de nivel

void VentanaJuego::mostrarFinDeNivel(int numero) {
    if (numero == 1) {
        QMessageBox::information(this, "¡Nivel completado!",
                                 "Abriste la bóveda y escapaste con el botín. ¡Ahora a huir en carro!");

        // La dificultad solo afecta al Nivel 2, así que se elige aquí (ya no al inicio)
        QMessageBox seleccion(this);
        seleccion.setWindowTitle("Dificultad de la fuga");
        seleccion.setText("Elige la dificultad del Nivel 2");
        seleccion.addButton("Fácil", QMessageBox::AcceptRole);
        QPushButton* dificil = seleccion.addButton("Difícil", QMessageBox::AcceptRole);
        seleccion.exec();
        gestor->establecerDificultad(seleccion.clickedButton() == dificil);
    }
    gestor->avanzar();
}

void VentanaJuego::mostrarDerrota(int numero) {
    const bool murio = numero == 1 && gestor->nivel1()
                       && gestor->nivel1()->murioPorLaser();
    const QString mensaje = murio ? "Moriste. ¿Quieres reintentar el nivel?"
                                  : "Te atraparon. ¿Quieres reintentar el nivel?";
    const auto respuesta = QMessageBox::question(this, "Derrota",
                                                 mensaje);
    if (respuesta == QMessageBox::Yes) {
        gestor->iniciarNivel(numero);
    } else {
        close();
    }
}

void VentanaJuego::mostrarVictoriaFinal() {
    QMessageBox::information(this, "¡Ganaron!", "Los Minions escaparon con el botín.");
    close();
}
