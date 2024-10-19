#include <iostream>
#include <bitset>  // Librería para bitset
using namespace std;

class Ram {
private:

    bitset<64> mem[256];

public:
    
    // Métodos para obtener los atributos
    bitset<64> read_mem(int reg_adr) {
        return mem[reg_adr];
    }

    void write_mem(int reg_addr, bitset<64> reg_val) {
        mem[reg_addr] = reg_val;
    }

};