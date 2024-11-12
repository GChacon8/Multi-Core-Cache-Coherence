#include "Ram.h"

// Método para leer un valor de la memoria en una dirección específica
uint64_t Ram::read_mem(int reg_adr) {
    return mem[reg_adr];
}

// Método para escribir un valor en una dirección específica de la memoria
void Ram::write_mem(int reg_addr, uint64_t reg_val) {
    mem[reg_addr] = reg_val;
}
