#include <QApplication>

#include "gui/GestorJuego.h"
#include "gui/VentanaJuego.h"

int main(int argc, char* argv[]) {
    QApplication aplicacion(argc, argv);

    GestorJuego gestor;
    VentanaJuego ventana(&gestor);
    ventana.show();

    return aplicacion.exec();
}
