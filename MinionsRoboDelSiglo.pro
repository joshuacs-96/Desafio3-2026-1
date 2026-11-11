QT += core

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = MinionsRoboDelSiglo

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/logica/TiposJuego.cpp \
    src/logica/Entidad.cpp \
    src/logica/MovimientoOscilatorio.cpp \
    src/logica/MovimientoParabolico.cpp \
    src/logica/MovimientoAmortiguado.cpp \
    src/logica/Personaje.cpp \
    src/logica/CarroFuga.cpp \
    src/logica/Objeto.cpp

HEADERS += \
    src/logica/TiposJuego.h \
    src/logica/Entidad.h \
    src/logica/Fisica.h \
    src/logica/MovimientoOscilatorio.h \
    src/logica/MovimientoParabolico.h \
    src/logica/MovimientoAmortiguado.h \
    src/logica/Personaje.h \
    src/logica/CarroFuga.h \
    src/logica/Objeto.h
