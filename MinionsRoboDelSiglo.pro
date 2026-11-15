QT += core

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = MinionsRoboDelSiglo

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/logica/Minion.cpp \
    src/logica/CarroJugador.cpp \
    src/logica/CarroPolicia.cpp \
    src/logica/Obstaculo.cpp \
    src/logica/Boveda.cpp \
    src/logica/Escalera.cpp \
    src/logica/CamaraSeguridad.cpp \
    src/logica/GuardiaCentinela.cpp \
    src/logica/Nivel.cpp \
    src/logica/Nivel1Banco.cpp \
    src/logica/Nivel2Fuga.cpp

HEADERS += \
    src/logica/Ruido.h \
    src/logica/Minion.h \
    src/logica/CarroJugador.h \
    src/logica/CarroPolicia.h \
    src/logica/Obstaculo.h \
    src/logica/Boveda.h \
    src/logica/Escalera.h \
    src/logica/CamaraSeguridad.h \
    src/logica/GuardiaCentinela.h \
    src/logica/Nivel.h \
    src/logica/Nivel1Banco.h \
    src/logica/Nivel2Fuga.h
