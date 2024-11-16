#include "ProcessingElement.h"

ProcessingElement::ProcessingElement(int _id, const string &rom_filename, BusInterconnect &bus)
    : id(_id),
      cache(_id, bus),
      rom(rom_filename),
      bus(bus)
{
    label_map = rom.get_label_map();
    reg[0] = 0;
    reg[1] = 0;
    reg[2] = 0;
    reg[3] = 0;
}

ProcessingElement::~ProcessingElement()
{
    stop();
}

int ProcessingElement::get_id() const
{
    return id;
}

int ProcessingElement::get_cache_id() const
{
    return cache.get_id();
}

uint64_t ProcessingElement::get_reg(int reg_num) const
{
    return reg[reg_num];
}

void ProcessingElement::set_reg(int reg_num, uint64_t value)
{
    if (reg_num >= 0 && reg_num < 4)
    {
        reg[reg_num] = value;
    } else {
        cout << "Error: índice de registro fuera de rango." endl;
    }
    
}

void ProcessingElement::INC(int reg_num)
{
    reg[reg_num] = reg[reg_num] + 1;
}

void ProcessingElement::DEC(int reg_num)
{
    reg[reg_num] = reg[reg_num] - 1;
}

void ProcessingElement::WRITE(int reg_num, uint8_t address)
{
    if (reg_num >= 0 && reg_num < 4)
    {
        cache.write(address, reg[reg_num]);
    } else {
        cout << "Error: índice de registro fuera de rango en READ." endl;
    }
    
}

void ProcessingElement::READ(int reg_num, uint8_t address)
{
    if (reg_num >= 0 && reg_num < 4)
    {
        reg[reg_num] = cache.read(address);
    } else {
        cout << "Error: índice de registro fuera de rango en READ." endl;
    }
    
}

void ProcessingElement::next()
{
    inst instruction = rom.get_instruction();

    if (instruction.inst == "END")
    {
        cout << "Fin del programa." << endl;
        return;
    }

    if (instruction.inst == "LOAD")
    {
        reg[instruction.reg_num] = cache.read(instruction.addr);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "STORE")
    {
        cache.write(instruction.addr, reg[instruction.reg_num]);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "INC")
    {
        INC(instruction.reg_num);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "DEC")
    {
        DEC(instruction.reg_num);
        prev_reg_num = instruction.reg_num;
    } else if (instruction.inst == "JNZ")
    {
        if (reg[prev_reg_num] != 0)
        {
            if (label_map.find(instruction.label) != label_map.end()) 
            {
                program_counter = label_map[instruction.label];
                cout << "se realizo un salto a: " << program_counter << endl;
                rom.branch(program_counter);
                return;
            } else {
                cout << "Error: etiqueta no encontrada: " << instruction.inst << endl;
            }
        }  
    } else {
        cout << "Instrucción desconocida: " << instruction.inst << endl;
    }
    program_counter++;
}

Cache ProcessingElement::get_cache()
{
    return Cache();
}

void ProcessingElement::start()
{
    running = true;
    peThread = thread(&ProcessingElement::run, this);
}

void ProcessingElement::stop()
{
    if (running)
    {
        running = false;
        if (peThread.joinable())
        {
            peThread.join();
        }
    }
}

void ProcessingElement::run()
{
    while (running)
    {
        next();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
}