cmake_minimum_required(VERSION 3.16)

project(MinionsRoboDelSiglo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(MinionsRoboDelSiglo
    src/main.cpp
    src/logica/TiposJuego.cpp
    src/logica/Entidad.cpp
    src/logica/MovimientoOscilatorio.cpp
    src/logica/MovimientoParabolico.cpp
    src/logica/MovimientoAmortiguado.cpp
    src/logica/Personaje.cpp
    src/logica/CarroFuga.cpp
    src/logica/Objeto.cpp
)

target_include_directories(MinionsRoboDelSiglo PRIVATE src)
target_link_libraries(MinionsRoboDelSiglo PRIVATE Qt${QT_VERSION_MAJOR}::Core)
