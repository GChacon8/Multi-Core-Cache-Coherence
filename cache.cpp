#include <iostream>
#include <bitset>  // Librería para bitset
using namespace std;

class Cache {
private:
    // Atributos
    int id;
    
    u_int64_t data[4][8];
    u_int8_t addr[4][8];
    bitset<2> state[4][8];
    u_int8_t miss_count[4][8];
    u_int8_t inv_count[4][8];

public:
    
    // Métodos para obtener los atributos
    u_int64_t get_data(int block_num, int pos) {
        return data[pos][block_num];
    }

    void set_data(int block_num, int pos, u_int64_t value) {
        data[pos][block_num] = value;
    }

};
