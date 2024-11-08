#include <iostream>
#include <bitset>  // Librería para bitset
#include "cache.cpp"
#include <cstdint> // For uint64_t
#include "ROM.cpp"
using namespace std;

class PE {
private:
    // Atributos
    int id;
    uint64_t reg[4];
    Cache cache;
    Rom rom;

public:
    // Constructor
    PE(int _id) {
        id = _id;
    }

    // Métodos para obtener los atributos
    int  get_id() {
        return id;
    }

    uint64_t get_reg(int reg_num) {
        return reg[reg_num];
    }

    void set_reg(int reg_num, uint64_t value) { //metodo para definir el contenido de un registro
        reg[reg_num] = value;
    }

    void INC(int reg_num){ // incremneto en 1 del registro
        reg[reg_num] = reg[reg_num] + 1;
    }

    void DEC(int reg_num){ // decremento en 1 del registro
        reg[reg_num] = reg[reg_num] - 1;
    }

    void WRITE(int reg_num, uint8_t addres){
        cache.write(addres,reg[reg_num]);
    }

};
