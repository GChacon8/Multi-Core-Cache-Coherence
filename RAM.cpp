#include <iostream>
#include <bitset>  // Librería para bitset
#include <cstdint> // For uint64_t
using namespace std;

class Ram {
private:

    uint64_t mem[256];

public:
    
    // Métodos para obtener los atributos
    uint64_t read_mem(int reg_adr) {
        return mem[reg_adr];
    }

    void write_mem(int reg_addr, uint64_t reg_val) {
        mem[reg_addr] = reg_val;
    }

};