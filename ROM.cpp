// ROM.cpp
#include "ROM.h"
#include <fstream>
#include <sstream>
#include <iostream>

void Rom::load_instructions(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string op;
        inst instruction;

        if (line.back() == ':') {  // Una línea con ":" es una etiqueta
            std::string label = line.substr(0, line.size() - 1);  // Remover ":"
            label_map[label] = line_number;  // Guardar la posición de la etiqueta
            continue;  // No procesar más esta línea
        }

        iss >> op;
        printf("LA OPERACION ES: ");
        printf("%s", op.c_str());
        printf("\n");

        if (op == "LOAD") {
            instruction.inst = "LOAD";
            iss >> instruction.reg_num >> instruction.addr;
            instruction.cond = false;
            instruction.label="";
        } else if (op == "STORE") {
            instruction.inst = "STORE";
            iss >> instruction.reg_num >> instruction.addr;
            instruction.cond = false;
            instruction.label="";
        } else if (op == "INC") {
            instruction.inst = "INC";
            iss >> instruction.reg_num;
            instruction.addr = 0;
            instruction.cond = false;
            instruction.label="";
        } else if (op == "DEC") {
            instruction.inst = "DEC";
            iss >> instruction.reg_num;
            instruction.addr = 0;
            instruction.cond = false;
            instruction.label="";
        } else if (op == "JNZ") {
            instruction.inst = "JNZ";
            std::string label;
            iss >> label;
            instruction.reg_num = 0;
            instruction.addr = 0;
            instruction.cond = true;
            instruction.label=label;
        } else {
            std::cout << "Error: Instrucción desconocida en la línea: " << line << std::endl;
            continue;
        }
        instructions.push_back(instruction);
        line_number++;
    }
}

inst Rom::get_instruction() {
    if (current_line < instructions.size()) {
        return instructions[current_line++];
    } else {
        return { "END", 0, 0, false };
    }
}

std::unordered_map<std::string, int> Rom::get_label_map() {
    return label_map;
}

void Rom::branch(int i) {
    current_line=i;
}


