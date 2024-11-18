#include <iostream>  // Librería para entrada/salida
#include <bitset>  // Librería para bitset
#include <cstdint>
#include <fstream>
#include "PE.cpp"
#include "Ram.h"
#include "ROM.h"
#include <thread>
using namespace std;


#include <unistd.h>

void pe_next(PE* core) {
    int counter = 0;
    while (true) {
        if(counter > 12) {
            break;
        }
        sleep(0.1);
        printf("PE next done\n");
        core->next();
        counter++;
    }
}

int main() {
    bool stepper = false;
    int numPEs = 2;
    Ram ram;
    for (int i = 0; i < 256; ++i) {
        ram.write_mem(i, i);
    }
    vector<Cache*> caches;
    BusInterconnect bus(ram, numPEs, caches);  // 4 es el número de PEs, por ejemplo
   


    PE core0 = PE(0, "C:/Users/joedu/OneDrive/Escritorio/Multi-Core-Cache-Coherence/ROM.txt", bus);
    PE core1 = PE(1, "C:/Users/joedu/OneDrive/Escritorio/Multi-Core-Cache-Coherence/ROM.txt", bus);

    caches.push_back(core0.get_cache());
    caches.push_back(core1.get_cache());

    caches.size();


    thread bus_thread(&BusInterconnect::processRequests, &bus);


    if (!stepper) {
        thread pe_thread0(pe_next, &core0);
        thread pe_thread1(pe_next, &core1);

        pe_thread0.join();
        pe_thread1.join();

        bus_thread.join();
        bus.stopBus = true;

        cout << "Invalidaciones:\t" << bus.getNumInvalidations() << endl;
        cout << "Read Requests:\t" << bus.getNumReadRequests() << endl;
        cout << "Read Responses:\t" << bus.getNumReadResponses() << endl;
        cout << "Write Requests:\t" << bus.getNumWriteRequests() << endl;
        cout << "Write Responses:\t" << bus.getNumWriteResponses() << endl;
        for(int i = 0; i < numPEs; i++) {
            cout << "Datos transmitidos al PE " << i << ":\t" << bus.getDataTransmitted(i) << " bytes" << endl;
        }

        core0.get_cache()->displayStats();
        core1.get_cache()->displayStats();

        return 0;


    } else {
        cout << "Presione 'i' para ver la informacion de los cores.\nPresione 'c' para el siguiente paso.\nPresione 'b' para salir.\nQue quiere hacer?: ";
        char input;
        while(true) {
            bus.stopBus = false;
            system("cls");
            cout << "Que quiere hacer?: " << endl;
            cin >> input;
            if (input == 'b') {
                bus.stopBus = true;
                break;
            };
            if (input == 'i') {
                //core0.get_cache()->displayStats();
                //core1.get_cache()->displayStats();
            } else if (input == 'c') {

                core0.next();
                core1.next();
                bus.stopBus = true;
            } else {
                cout << "Comando desconocido";
            }
        }
        bus_thread.join();

        cout << "Invalidaciones:\t" << bus.getNumInvalidations() << endl;
        cout << "Read Requests:\t" << bus.getNumReadRequests() << endl;
        cout << "Read Responses:\t" << bus.getNumReadResponses() << endl;
        cout << "Write Requests:\t" << bus.getNumWriteRequests() << endl;
        cout << "Write Responses:\t" << bus.getNumWriteResponses() << endl;
        for(int i = 0; i < numPEs; i++) {
            cout << "Datos transmitidos al PE " << i << ":\t" << bus.getDataTransmitted(i) << " bytes" << endl;
        }

        core0.get_cache()->displayStats();
        core1.get_cache()->displayStats();

        return 0;
    }
}
