#ifndef PE_H
#define PE_H

#include <iostream>
#include <bitset>
#include <cstdint>
#include <unistd.h>
#include <unordered_map>
#include "ROM.h"
#include "BusInterconnect.h"
#include "Cache.h"

class PE {
private:
    int id;                                                                 // Processing Element (PE) ID
    uint64_t reg[4];                                                        // General-purpose registers
    Cache* cache;                                                           // Cache associated with the PE
    std::unordered_map<std::string, int> label_map;                         // Map of labels to ROM lines
    int program_counter;                                                    // Program counter
    int prev_reg_num;                                                       // Last modified register
    Rom rom;                                                                // ROM associated with the PE
    BusInterconnect& bus;                                                   // Bus interconnection reference

public:
    // Constructor
    PE(int _id, const std::string& rom_filename, BusInterconnect& bus);

    void INC(int reg_num);                                                  // Increment the value of a register
    void DEC(int reg_num);                                                  // Decrement the value of a register

    // Cache operations
    void WRITE(int reg_num, uint8_t address);                               // Write a register's value to the cache
    void READ(int reg_num, uint8_t address);                                // Read a value from the cache into a register

    // Instruction execution
    void next();                                                            // Execute the next instruction

    Cache* get_cache();                                                     // Get the cache associated with the PE
    int getMyRomSize();                                                     // Get the size of the ROM
    int get_id();                                                           // Get the PE ID
    int get_cache_id();                                                     // Get the cache ID
    uint64_t get_reg(int reg_num);                                          // Get the value of a specific register
    void set_reg(int reg_num, uint64_t value);                              // Set the value of a specific register
};

#endif // PE_H








void next() {
    inst instruction = rom.get_instruction();  // Fetch The next Instruction
    if (instruction.inst == "END") {
        cout << "Program finished." << endl;
        return;
    }
    if (instruction.inst == "LOAD") {                               // Load the value from the address into the corresponding register
        reg[instruction.reg_num] = cache->read(instruction.addr);  
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "STORE") {                       // Store the value of the register at the specified address
        cache->write(instruction.addr, reg[instruction.reg_num]);  
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "INC") {                         // Increment the value of the register
        INC(instruction.reg_num);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "DEC") {                         // Decrement the value of the register
        DEC(instruction.reg_num);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "JNZ") {                         // Jump if the previous register is not zero
        
        if (reg[prev_reg_num] != 0) {
            if (label_map.find(instruction.label) != label_map.end()) {
                program_counter = label_map[instruction.label];  // Update program_counter to the label's line
                cout << "Jumped to: " << program_counter << endl;
                rom.branch(program_counter);
                return;  // Do not increment program_counter since we jumped
            } else {
                cout << "Error: Label not found: " << instruction.inst << endl;
            }
        }
    } else {
        cout << "Unknown instruction." << endl;
    }
    // Increment the program counter to the next instruction
    program_counter++;
}

