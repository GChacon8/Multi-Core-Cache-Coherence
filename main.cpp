#include <iostream>  // Librería para entrada/salida
#include <bitset>  // Librería para bitset
#include <cstdint>
#include <fstream>
#include "PE.cpp"
#include "Ram.h"
#include "ROM.h"
using namespace std;


#include <unistd.h>



int main() {
    Ram ram;
    vector<Cache*> caches;
    BusInterconnect bus(ram, 4, caches);  // 4 es el número de PEs, por ejemplo
    // Actualización de las rutas a la nueva ubicación de los archivos
    PE core1 = PE(1, "ROM.txt", bus);
    //PE core2 = PE(2);
    //PE core3 = PE(3);
    //PE core4 = PE(4);

    // Instancia la ROM, pasando el nombre del archivo de instrucciones
    //Rom rom(R"(~/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROM.txt)");

    // Puedes utilizar el objeto rom para obtener instrucciones
    //inst current_instruction = rom.get_instruction();

    printf("SE EJECUTÓ!!!");

    /*while (current_instruction.inst != "END") { // -1 indica el fin de las instrucciones
        // Procesar la instrucción actual
        std::cout << "Instruccion: " << current_instruction.inst << ", "
                  << "Registro: " << current_instruction.reg_num << ", "
                  << "Direccion: " << current_instruction.addr << ", "
                  << "Condicion: " << current_instruction.cond << std::endl;

        // Obtener la siguiente instrucción
        current_instruction = rom.get_instruction();
    }*/

    core1.next();
    sleep(2);
    core1.next();
    sleep(2);
    core1.next();
    sleep(2);
    core1.next();
    sleep(2);
    core1.next();
    sleep(2);
    core1.next();
    sleep(2);
    core1.next();
}
