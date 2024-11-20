#include <iostream>
#include <bitset>  // Librería para bitset
#include <cstdint> // For uint64_t
#include <unistd.h>

#include "ROM.h"
#include "BusInterconnect.h"
#include <unordered_map>
using namespace std;

class PE {
private:
    // Atributos
    int id;
    uint64_t reg[4];
    Cache* cache;
    unordered_map<string, int> label_map; // Mapea etiquetas a líneas en ROM
    int program_counter = 0; // Posición actual de la instrucción en ROM
    int prev_reg_num = 0;
    // mutex PE_mutex;
    Rom rom;
    BusInterconnect& bus;

    int executedInstructions;

public:
    // Constructor
    PE(int _id,const std::string& rom_filename, BusInterconnect& bus) : id(_id), cache(new Cache(_id,bus)),rom(rom_filename), bus(bus), executedInstructions(0) {
        label_map=rom.get_label_map();
        reg[0]=0;
        reg[1]=0;
        reg[2]=0;
        reg[3]=0;

    }

    // Métodos para obtener los atributos
    int  get_id() {
        return id;
    }

    int get_cache_id(){
        return cache->get_id();
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
            cache->write(address,reg[reg_num]); //escribe los valores en la cache
        } else {
            cout << "Error: índice de registro fuera de rango en READ.\n";
        }
    }

    void READ(int reg_num, uint8_t address){
        if (reg_num >= 0 && reg_num < 4) {  // Verificar que reg_num está dentro de los límites
            reg[reg_num] = cache->read(address);  // Leer el valor de la caché y guardarlo en el registro
        } else {
            cout << "Error: índice de registro fuera de rango en READ.\n";
        }
    }

    void next() {
        // Obtener la siguiente instrucción de la ROM
        inst instruction = rom.get_instruction();  // Suponiendo que tienes acceso a un objeto ROM

        cout<<"Ejecutando instruccion: " << program_counter << " con la instruccion: " << instruction.inst;
        // Si la instrucción es "END", terminamos
        if (instruction.inst == "END") {
            cout << "Fin del programa." << endl;
            return;
        }

        // Ejecutar la instrucción según el tipo
        if (instruction.inst == "LOAD") {
            // Cargar valor de la dirección en el registro correspondiente
            reg[instruction.reg_num] = cache->read(instruction.addr);  // Asumiendo que cache.read() carga el valor de la memoria.
            prev_reg_num = instruction.reg_num;
            executedInstructions++;
        } else if (instruction.inst == "STORE") {
            // Almacenar el valor del registro en la dirección indicada
            cache->write(instruction.addr, reg[instruction.reg_num]);  // Suponiendo que cache.write() escribe en la memoria.
            prev_reg_num = instruction.reg_num;
            executedInstructions++;
        } else if (instruction.inst == "INC") {
            // Incrementar el valor del registro
            INC(instruction.reg_num);
            prev_reg_num = instruction.reg_num;
            executedInstructions++;
        } else if (instruction.inst == "DEC") {
            // Decrementar el valor del registro
            DEC(instruction.reg_num);
            prev_reg_num = instruction.reg_num;
            executedInstructions++;
        } else if (instruction.inst == "JNZ") {
            // Saltar si el registro no es cero
            if (reg[prev_reg_num] != 0) {
                // Buscar la etiqueta en el mapeo de etiquetas
                if (label_map.find(instruction.label) != label_map.end()) {
                    program_counter = label_map[instruction.label];  // Actualizar el program_counter con la línea de la etiqueta
                    cout << "se realizo un salto a: " << program_counter << endl;
                    rom.branch(program_counter);
                    executedInstructions++;
                    return;  // No se debe avanzar en el program_counter, ya que saltamos
                } else {
                    cout << "Error: etiqueta no encontrada: " << instruction.inst << endl;
                }
            }
        } else {
            cout << "Instrucción desconocida." << endl;
        }

        // Avanzar el program_counter al siguiente
        program_counter++;
    }

    Cache* get_cache(){
        return cache;
    }

    int getMyRomSize() {
        return rom.getRomSize();
    }

    int getExecutedInstructions() const {
        return executedInstructions;
    }
};
