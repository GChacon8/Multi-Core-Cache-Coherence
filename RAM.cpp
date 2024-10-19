#include <iostream>
#include <bitset>  // Librería para bitset
using namespace std;

class Ram {
private:

    u_int64_t mem[256];

public:
    
    // Métodos para obtener los atributos
    u_int64_t read_mem(int reg_adr) {
        return mem[reg_adr];
    }

    void write_mem(int reg_addr, u_int64_t reg_val) {
        mem[reg_addr] = reg_val;
    }

};