#include <iostream>
#include <bitset>  // Librería para bitset
#include "cache.cpp"
#include "ROM.cpp"
using namespace std;

class PE {
private:
    // Atributos
    int id;
    u_int64_t reg[4];
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

    u_int64_t get_reg(int reg_num) {
        return reg[reg_num];
    }

    void set_reg(int reg_num, u_int64_t value) {
        reg[reg_num] = value;
    }

    void INC(int reg_num){
        reg[reg_num] = reg[reg_num] + 1;
    }

    void DEC(int reg_num){
        reg[reg_num] = reg[reg_num] - 1;
    }

};
