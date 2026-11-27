#pragma once
#include <QSizeF>

// Una animación hecha con cuadros sueltos numerados (base1.png, base2.png, ...)
struct Animacion {
    const char* base;      // ruta dentro del .qrc, sin número ni ".png"
    int cuadros;           // cuántos cuadros tiene (1 = un solo archivo, sin número)
    double fps;            // cuadros por segundo (0 = no se anima sola)
    int ancho;             // tamaño de un cuadro (solo para el relleno si falta el PNG)
    int alto;
    bool primeroSinNumero = false;   // true si el 1.er cuadro es "base" y los demás "base1", "base2"...
};

namespace ConfigGrafica {

// Tamaño del "mundo" que usa la lógica
constexpr double ANCHO_LOGICO_1 = 400.0;
constexpr double ALTO_LOGICO_1  = 320.0;
constexpr double ANCHO_LOGICO_2 = 400.0;
constexpr double ALTO_LOGICO_2  = 500.0;

constexpr double RADIO_INTERACCION_PUERTA = 60.0;
constexpr double DANIO_PUERTA_POR_SEGUNDO = 20.0;

// ¿Hacia dónde miran tus dibujos? true = a la derecha, false = a la izquierda
constexpr bool MINION_MIRA_DERECHA  = true;
constexpr bool GUARDIA_MIRA_DERECHA = true;

// Escaleras: ponlo en false si tu Escenario_N1 ya trae las escaleras dibujadas
constexpr bool DIBUJAR_ESCALERAS = true;
constexpr double ANCHO_ESCALERA = 70.0;

// Tamaño de cada sprite en la escena (px). El dibujo se ajusta a esa caja sin deformarse.
inline const QSize TAM_MINION(84, 84);
inline const QSizeF TAM_MINION_FUERTE(90, 90);
inline const QSizeF TAM_GUARDIA(90, 130);
inline const QSizeF TAM_ICONO(30, 30);
inline const QSizeF TAM_CAMARA(48, 32);
inline const QSizeF TAM_PUERTA(96, 128);
inline const QSizeF TAM_BANANA(35, 35);
inline const QSizeF TAM_LINGOTE(45, 30);
inline const QSizeF TAM_BOLSA(45, 45);
inline const QSizeF TAM_CARRO(64, 128);
inline const QSizeF TAM_OBSTACULO(64, 64);

namespace Anim {

// ---------------- Fondos ----------------
inline constexpr Animacion FONDO_NIVEL1 {"Sprits d3/escenarios/Escenario_N1", 1, 0.0, 1280, 720};
inline constexpr Animacion FONDO_NIVEL2 {"Sprits d3/escenarios/Escenario_N2", 1, 0.0, 640, 960};

// ---------------- Kevin ----------------
inline constexpr Animacion MINION_IDLE      {"Sprits d3/minions/minion_estatico", 1, 0.0, 64, 64};
inline constexpr Animacion MINION_ELECTROCUTADO {"Sprits d3/minions/minion_electrocutado", 1, 0.0, 64, 64};
inline constexpr Animacion MINION_CAMINAR   {"Sprits d3/minions/minions_caminando", 4, 8.0, 64, 64};
inline constexpr Animacion MINION_SALTAR    {"Sprits d3/minions/minions_saltando", 3, 0.0, 64, 64};
inline constexpr Animacion MINION_GOLPEAR   {"Sprits d3/minions/minions_martillo", 4, 8.0, 64, 64, true};
inline constexpr Animacion MINION_ESCALERA  {"Sprits d3/minions/minions_subiendoescalera", 2, 6.0, 64, 64};   // (reservado)
inline constexpr Animacion MINION_RISA      {"Sprits d3/minions/minions_riendose", 1, 0.0, 64, 64};           // (reservado)
inline constexpr Animacion MINION_GOLPEADO  {"Sprits d3/minions/minions_golpeado", 1, 0.0, 64, 64};           // (reservado)
inline constexpr Animacion MINION_CAPTURADO {"Sprits d3/minions/minions_capturado", 1, 0.0, 64, 64};          // (reservado)

// ---------------- Kevin fortachón ----------------
inline constexpr Animacion FORTACHON_IDLE    {"Sprits d3/minions/Fortachon_estatico", 1, 0.0, 64, 64};
inline constexpr Animacion FORTACHON_CAMINAR {"Sprits d3/minions/Fortachon_caminando", 3, 8.0, 64, 64};
inline constexpr Animacion FORTACHON_GOLPEAR {"Sprits d3/minions/minions_martillo", 4, 8.0, 64, 64, true};

// ---------------- Policía (agente) ----------------
inline constexpr Animacion POLICIA_QUIETO    {"Sprits d3/agente/policia_quieto", 1, 0.0, 64, 96};
inline constexpr Animacion POLICIA_CAMINAR   {"Sprits d3/agente/policia_caminando", 3, 7.0, 64, 96};
inline constexpr Animacion POLICIA_CORRER    {"Sprits d3/agente/policia_caminando", 3, 12.0, 64, 96};   // mismos cuadros, más rápido
inline constexpr Animacion POLICIA_ATRAPANDO {"Sprits d3/agente/policia_atrapando", 1, 0.0, 64, 96};
inline constexpr Animacion SIGNO_ADMIRACION    {"Sprits d3/agente/signo_admiracion", 1, 0.0, 24, 24};
inline constexpr Animacion SIGNO_INTERROGACION {"Sprits d3/agente/signo_interrogacion", 1, 0.0, 24, 24};

// ---------------- Nivel 1: objetos ----------------
inline constexpr Animacion PUERTA_CERRADA {"Sprits d3/objetos_N1/boveda", 1, 0.0, 96, 128};
inline constexpr Animacion PUERTA_DANADA  {"Sprits d3/objetos_N1/boveda_danada", 1, 0.0, 96, 128};
inline constexpr Animacion PUERTA_ABIERTA {"Sprits d3/objetos_N1/boveda_abierta", 1, 0.0, 96, 128};
inline constexpr Animacion CAMARA    {"Sprits d3/objetos_N1/camara", 1, 0.0, 48, 32};
inline constexpr Animacion ESCALERA  {"Sprits d3/objetos_N1/escaleras", 1, 0.0, 64, 100};
inline constexpr Animacion ESCALERA_ROJA {"Sprits d3/objetos_N1/escaleras_roja", 1, 0.0, 64, 100};
inline constexpr Animacion BANANA    {"Sprits d3/objetos_N1/banana", 1, 0.0, 32, 32};
inline constexpr Animacion LINGOTE   {"Sprits d3/objetos_N1/oro", 1, 0.0, 40, 24};
inline constexpr Animacion BOLSA     {"Sprits d3/objetos_N1/bolsa", 1, 0.0, 40, 40};
// reservados para el láser y los objetos que caen:
inline constexpr Animacion RAYO_LASER  {"Sprits d3/objetos_N1/RayoLaser", 1, 0.0, 16, 64};
inline constexpr Animacion SOPORTE {"Sprits d3/objetos_N1/soporte", 1, 0.0, 48, 48};
inline constexpr Animacion SOPORTE_DESTRUIDO {"Sprits d3/objetos_N1/soporte_destruido", 1, 0.0, 48, 48};
inline constexpr Animacion SOPORTE_DESTRUIDO_ROJO {"Sprits d3/objetos_N1/soporte_destruido_rojo", 1, 0.0, 48, 48};
inline constexpr Animacion CAJA_CARTON {"Sprits d3/objetos_N1/caja_carton", 1, 0.0, 48, 48};
inline constexpr Animacion JARRON      {"Sprits d3/objetos_N1/jarron", 1, 0.0, 48, 48};
inline constexpr Animacion MALETIN     {"Sprits d3/objetos_N1/maletin", 1, 0.0, 48, 48};

// ---------------- Nivel 2 ----------------
inline constexpr Animacion CARRO_NORMAL     {"Sprits d3/objetos_N2/CarroPersecucion", 1, 0.0, 64, 128};
inline constexpr Animacion CARRO_SEMIDANADO {"Sprits d3/objetos_N2/CarroPersecucion_semidanado", 1, 0.0, 64, 128};
inline constexpr Animacion CARRO_DANADO     {"Sprits d3/objetos_N2/CarroPersecucion_danado", 1, 0.0, 64, 128};
inline constexpr Animacion CARRO_POLICIA    {"Sprits d3/objetos_N2/CarroPersecucion", 1, 0.0, 64, 128};
inline constexpr Animacion OBS_CONO     {"Sprits d3/objetos_N2/cono", 1, 0.0, 64, 64};
inline constexpr Animacion OBS_BARRERA  {"Sprits d3/objetos_N2/cono", 1, 0.0, 96, 40};
inline constexpr Animacion OBS_HUECO    {"Sprits d3/objetos_N2/hueco", 1, 0.0, 64, 64};
inline constexpr Animacion OBS_ACEITE   {"Sprits d3/objetos_N2/machaAceite", 1, 0.0, 96, 96};
inline constexpr Animacion CHISPAS      {"Sprits d3/objetos_N2/chispasChoque", 1, 0.0, 64, 64};   // (reservado)
inline constexpr Animacion BOLSA_CALLE  {"Sprits d3/objetos_N2/bolsadebotin", 1, 0.0, 40, 40};    // (reservado)

}

}
