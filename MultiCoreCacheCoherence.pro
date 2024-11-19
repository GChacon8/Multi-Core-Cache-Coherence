QT += core gui widgets

CONFIG += c++11

# Archivos fuente
SOURCES += main.cpp \
           PE.cpp \
           cache.cpp \
           Ram.cpp \
           ROM.cpp \
           BusInterconnect.cpp \
           MESI.cpp

# Archivos de encabezado
HEADERS += cache.h \
           Ram.h \
           ROM.h \
           BusInterconnect.h \

# Otros
RESOURCES += 
