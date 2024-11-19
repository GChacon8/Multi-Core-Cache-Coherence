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
    cout<<"SE EJECUTA PRIMERO EL HILO DEL CORE"<<core->get_id()<<endl;
    int counter = 0;
    int ROM_size = core->getMyRomSize();
    while (true) {
        if(counter > ROM_size) {
            printf("Termino el hilo en: %d\n", core->get_id());
            fflush(stdout);
            break;
        }
        sleep(0.1);
        core->next();
        counter++;
    }
}

int main() {
    int numPEs = 2;
    Ram ram;
    for (int i = 0; i < 256; i++) {
        ram.write_mem(i, 0);
    }
    vector<Cache*> caches;
    BusInterconnect bus(ram, numPEs, caches);
    cout << "Bienvenido al Simulador MESI" << endl;
    cout << "Elaborado por: \n- Daniel Casto Elizondo\n- Gabriel Chacon Alfaro\n- Jose Eduardo Cruz Vergas\n- Marco Vinicio Rivera Serrano" << endl;
    cout << "El simulador consiste en lograr visualizar los pasos si lo deseas o realizar la ejecucion directa de un archivo ROM de 4 elementos de procesamiento." << endl;
    system("pause");
    system("cls");

    char stepper_input;
    cout << "Deseas agregar un stepper? (y/n): ";
    cin >> stepper_input;
    bool stepper = (stepper_input == 'y') ? true : false;

    // C:/Users/joedu/OneDrive/Escritorio/Multi-Core-Cache-Coherence/ROM.txt
    // F:/Progras/Arqui II - Proyecto II/Multi-Core-Cache-Coherence/ROM.txt

    PE core0 = PE(0,"F:/Progras/Arqui II - Proyecto II/Multi-Core-Cache-Coherence/ROMPE0.txt", bus);
    PE core1 = PE(1,"F:/Progras/Arqui II - Proyecto II/Multi-Core-Cache-Coherence/ROMPE1.txt", bus);
    //PE core2 = PE(2,"F:/Progras/Arqui II - Proyecto II/Multi-Core-Cache-Coherence/ROM0.txt", bus);
    //PE core3 = PE(3,"F:/Progras/Arqui II - Proyecto II/Multi-Core-Cache-Coherence/ROM1.txt", bus);

    caches.push_back(core0.get_cache());
    caches.push_back(core1.get_cache());
    //caches.push_back(core2.get_cache());
    //caches.push_back(core3.get_cache());


    thread bus_thread(BusInterconnect::processRequests, &bus);

    if(!stepper) {
        thread pe_thread0(pe_next, &core0);
        thread pe_thread1(pe_next, &core1);
        //thread pe_thread2(pe_next, &core2);
        //thread pe_thread3(pe_next, &core3);

        while (!pe_thread0.joinable() && !pe_thread1.joinable() /*&& !pe_thread2.joinable() && !pe_thread3.joinable()*/) {
            continue;
        }
        pe_thread0.join();
        pe_thread1.join();
        //pe_thread2.join();
        //pe_thread3.join();

        bus.stopBus = true;
        this_thread::sleep_for(chrono::milliseconds(500));
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
        //core2.get_cache()->displayStats();
        //core3.get_cache()->displayStats();

        return 0;

    } else {
        cout << "Presione 'i' para ver la informacion de los cores.\nPresione 'c' para el siguiente paso.\nPresione 'b' para salir.\n";
        while(true) {
            system("cls");
            char user_selection;
            cout << "Que quiere hacer?: ";
            cin >> user_selection;
            if(user_selection == 'b') {
                bus.requestQueue.clear();
                bus.stopBus = true;
                break;
            } else if(user_selection == 'i') {
                core0.get_cache()->displayStats();
                core1.get_cache()->displayStats();
                //core2.get_cache()->displayStats();
                //core3.get_cache()->displayStats();
                cout << "\n";
            } else if (user_selection == 'c') {
                core0.next();
                core1.next();
                //core2.next();
                //core3.next();
            } else {
                cout << "Comando desconocido." << endl;
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
        //core2.get_cache()->displayStats();
        //core3.get_cache()->displayStats();

        return 0;
    }

}
