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
    PE(int _id) : id(_id), cache(_id){
        reg[3] = {0};
    }

    // Métodos para obtener los atributos
    int  get_id() {
        return id;
    }

    int get_cache_id(){
        return cache.get_id();
    };

    uint64_t get_reg(int reg_num) {
        return reg[reg_num];
    }

    void set_reg(int reg_num, uint64_t value) { //metodo para definir el contenido de un registro
        if (reg_num >= 0 && reg_num < 4) {  // Verificar que reg_num está entre 0 y 3
            reg[reg_num] = value;
        } else {
            cout << "Error: índice de registro fuera de rango.\n";
        }
    }

    void INC(int reg_num){ // incremneto en 1 del registro
        reg[reg_num] = reg[reg_num] + 1;
    }

    void DEC(int reg_num){ // decremento en 1 del registro
        reg[reg_num] = reg[reg_num] - 1;
    }

    void WRITE(int reg_num, uint8_t address){
        if (reg_num >= 0 && reg_num < 4) {  // Verificar que reg_num está dentro de los límites
            cache.write(address,reg[reg_num]); //escribe los valores en la cache
        } else {
            cout << "Error: índice de registro fuera de rango en READ.\n";
        }
    }

    void READ(int reg_num, uint8_t address){
        if (reg_num >= 0 && reg_num < 4) {  // Verificar que reg_num está dentro de los límites
            reg[reg_num] = cache.read(address);  // Leer el valor de la caché y guardarlo en el registro
        } else {
            cout << "Error: índice de registro fuera de rango en READ.\n";
        }
    }

};
