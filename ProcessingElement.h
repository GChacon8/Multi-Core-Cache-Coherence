#ifndef PROCESSING_ELEMENT_H
#define PROCESSING_ELEMENT_H

#include <iostream>
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include "ROM.h"
#include "BusInterconnect.h"
#include "cache.h"

bool stepper_mode = false;
bool stepper_ready = false;
mutex stepper_mutex;
condition_variable stepper_cv;

class ProcessingElement {
public:
private:
    // Atributos
    int id;
    uint64_t reg[4];
    Cache cache;
    unordered_map<string, int> label_map;
    int program_counter = 0;
    int prev_reg_num = 0;
    Rom rom;
    BusInterconnect& bus;

    // Metodos para el hilo
    thread peThread;
    bool running;

public:
    ProcessingElement(int _id, const string& rom_filename, BusInterconnect& bus);
    ~ProcessingElement();

    void start();
    void stop();

    int get_id() const;
    int get_cache_id() const;
    uint64_t get_reg(int reg_num) const;
    void set_reg(int reg_num, uint64_t value);

    void INC(int reg_num);
    void DEC(int reg_num);
    void WRITE(int reg_num, uint8_t address);
    void READ(int reg_num, uint8_t address);
    void next();
    Cache get_cache();
    void run();
};

#endif // PROCESSING_ELEMENT_H