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
    src/logica/Obstaculo.cpp

HEADERS += \
    src/logica/Ruido.h \
    src/logica/Minion.h \
    src/logica/CarroJugador.h \
    src/logica/Obstaculo.h
