cmake_minimum_required(VERSION 3.10)

project(MultiCoreCacheCoherence)

# Especifica el estándar de C++
set(CMAKE_CXX_STANDARD 11)

# Agrega los archivos fuente
set(SOURCES
    main.cpp
    BusInterconnect.cpp
    PE.cpp
    ROM.cpp
    cache.cpp
    Ram.cpp
    # Otros archivos .cpp que estés usando
)

# Agrega el ejecutable
add_executable(MultiCoreCacheCoherence ${SOURCES})

# Si tienes dependencias externas, agrega las rutas de enlace aquí
# target_link_libraries(MultiCoreCacheCoherence <nombre de las librerías>)
