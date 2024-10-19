#include <iostream>
#include <bitset>  // Librería para bitset
using namespace std;

class Cache {
private:
    // Atributos
    int id;
    bitset<256> mem[8];

public:
    
    // Métodos para obtener los atributos
    bitset<256> get_block(int block_num) {
        return mem[block_num];
    }

    void set_block(int block_num, bitset<256> block) {
        mem[block_num] = block;
    }

};
