#include <iostream>
#include <string>

enum MESIState {
    INVALID,   // I
    SHARED,    // S
    EXCLUSIVE, // E
    MODIFIED   // M
};

enum Event {
    READ_MISS,   // Miss al leer desde otra caché
    READ_HIT,    // Hit al leer desde la misma caché
    WRITE_HIT,   // Escritura válida en esta caché
    WRITE_MISS,  // Escritura inválida (requiere invalidez de otras cachés)
    INVALIDATE   // Invalida la línea en esta caché
};

class MESI {
private:
    MESIState state;  // Estado actual de la línea en la caché

public:
    // Constructor: inicia la línea en el estado INVALID
    MESI() : state(INVALID) {}

    // Método para obtener el nombre del estado actual
    std::string getStateName() const {
        switch (state) {
            case INVALID:   return "INVALID";
            case SHARED:    return "SHARED";
            case EXCLUSIVE: return "EXCLUSIVE";
            case MODIFIED:  return "MODIFIED";
            default:        return "UNKNOWN";
        }
    }

    // Método que maneja la transición según el evento recibido
    void handleEvent(Event event) {
        switch (state) {
            case INVALID:
                if (event == READ_MISS) {
                    state = SHARED; // Transición a SHARED después de un READ_MISS
                } else if (event == WRITE_MISS) {
                    state = MODIFIED;   // Transición a MODIFIED para un WRITE_MISS
                }
                break;

            case SHARED:
                if (event == READ_HIT) {
                    // Estado permanece SHARED
                } else if (event == WRITE_HIT) {
                    state = MODIFIED;   // Escritura exclusiva, cambio a MODIFIED
                } else if (event == INVALIDATE) {
                    state = INVALID;    // Invalidación por otra caché
                }
                break;

            case EXCLUSIVE:
                if (event == READ_HIT) {
                    // Estado permanece EXCLUSIVE
                } else if (event == WRITE_HIT) {
                    state = MODIFIED;   // Escritura, cambio a MODIFIED
                } else if (event == INVALIDATE) {
                    state = INVALID;    // Invalidación externa
                } else if (event == READ_MISS) {
                    state = SHARED;     // Cambio a SHARED si otra caché lee
                }
                break;

            case MODIFIED:
                if (event == INVALIDATE) {
                    state = INVALID;    // Invalidación externa
                } else if (event == READ_MISS) {
                    state = SHARED;     // Cambio a SHARED después de un READ_MISS
                }
                break;
        }
    }
};
