#ifndef RAM_H
#define RAM_H

#include <cstdint> // Para uint64_t

class Ram { 
private:
    uint64_t mem[256];  // Memoria de 256 bloques de 64 bits

public:
    // Constructor por defecto
    Ram() = default;

    // Método para leer un valor de la memoria en una dirección específica
    uint64_t read_mem(int reg_adr);

    // Método para escribir un valor en una dirección específica de la memoria
    void write_mem(int reg_addr, uint64_t reg_val);
};

#endif // RAM_H
