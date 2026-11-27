
QT += core gui widgets multimedia

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = MinionsRoboDelSiglo

INCLUDEPATH += $$PWD src src/gui

RESOURCES += recursos.qrc

SOURCES += \
    src/gui/GestorJuego.cpp \
    src/gui/GestorRecursos.cpp \
    src/gui/SpriteAnimado.cpp \
    src/gui/VentanaJuego.cpp \
    src/logica/Edificio.cpp \
    src/logica/ItemRecolectable.cpp \
    src/main.cpp \
    src/logica/Boveda.cpp \
    src/logica/CamaraSeguridad.cpp \
    src/logica/CarroJugador.cpp \
    src/logica/CarroPolicia.cpp \
    src/logica/Escalera.cpp \
    src/logica/GuardiaCentinela.cpp \
    src/logica/Minion.cpp \
    src/logica/Nivel.cpp \
    src/logica/Nivel1Banco.cpp \
    src/logica/Nivel2Fuga.cpp \
    src/logica/Obstaculo.cpp

HEADERS += \
    src/gui/ConfigGrafica.h \
    src/gui/GestorJuego.h \
    src/gui/GestorRecursos.h \
    src/gui/SpriteAnimado.h \
    src/gui/VentanaJuego.h \
    src/logica/Boveda.h \
    src/logica/CamaraSeguridad.h \
    src/logica/CarroJugador.h \
    src/logica/CarroPolicia.h \
    src/logica/Edificio.h \
    src/logica/Escalera.h \
    src/logica/GuardiaCentinela.h \
    src/logica/ItemRecolectable.h \
    src/logica/Minion.h \
    src/logica/Nivel.h \
    src/logica/Nivel1Banco.h \
    src/logica/Nivel2Fuga.h \
    src/logica/Obstaculo.h \
    src/logica/Ruido.h
