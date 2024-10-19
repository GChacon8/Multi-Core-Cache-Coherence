#include <iostream>
#include <bitset>  // Librería para bitset
#include "cache.cpp"
#include "ROM.cpp"
using namespace std;

class PE {
private:
    // Atributos
    int id;
    bitset<64> reg0 = 0;
    bitset<64> reg1 = 0;
    bitset<64> reg2 = 0;
    bitset<64> reg3 = 0;
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

    bitset<64> get_reg0() {
        return reg0;
    }

    bitset<64> get_reg1() {
        return reg1;
    }

    bitset<64> get_reg2() {
        return reg2;
    }

    bitset<64> get_reg3() {
        return reg3;
    }

    void set_reg0(bitset<64> value) {
        reg0 = value;
    }

    void set_reg1(bitset<64> value) {
        reg1 = value;
    }

    void set_reg2(bitset<64> value) {
        reg2 = value;
    }

    void set_reg3(bitset<64> value) {
        reg3 = value;
    }

};
