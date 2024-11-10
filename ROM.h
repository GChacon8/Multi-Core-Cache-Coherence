// ROM.h
#ifndef ROM_H
#define ROM_H

#include <iostream>
#include <string>
#include <vector>
#include "unordered_map"

struct inst {
    std::string inst;       // Código de la instrucción
    int reg_num;    // Número del registro
    int addr;       // Dirección de memoria
    bool cond;      // Condición
    std::string label;
};

class Rom {
private:
    std::vector<inst> instructions;
    int current_line = 0;
    std::unordered_map<std::string, int> label_map;
    int line_number = 0;

public:
    Rom(const std::string& filename) {
        load_instructions(filename);
    }

    void load_instructions(const std::string& filename);
    inst get_instruction();
    void reset() { current_line = 0; }
    std::unordered_map<std::string,int> get_label_map();
    void branch(int i);
};

#endif // ROM_H
