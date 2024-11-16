#include "Ram.h"
#include "BusInterconnect.h"
#include "ProcessingElement.h"
#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Ram sharedMemory;
    BusInterconnect bus;

    int numPEs = 4;
    BusInterconnect bus(sharedMemory, numPEs);

    vector<thread> pe_threads;
    vector<ProcessingElement> pes;
    // Agregar la ROM
    for (int i = 0; i < numPEs; i++)
    {
        pes.emplace_back(ProcessingElement(i, &bus));
        pe_threads.emplace_back(&ProcessingElement::run, &pes.back())
    }
    
    thread bus_thread(&BusInterconnect::processRequests, &bus);

    this_thread::sleep_for(chrono::seconds(1));

    stepper_mode = true;

    char input;
    while(true)
    {
        cout << "Presione 'c' para el siguiente paso o 'q' para salir: ";
        cin >> input;

        if (input == 'q') break;

        if (input == 'c')
        {
            {
                unique_lock<mutex> lock(stepper_mutex);
                stepper_ready = true;
            }
            stepper_cv.notify_all();
        }
        
    }

    for (auto& pe : pes)
    {
        pe.stop();
    }

    for (auto& pe_t : pe_threads)
    {
        if (pe_t.joinable())
        {
            pe_t.join();
        }
    }

    bus.stopBus = true;
    if(bus_thread.joinable()) {
        bus_thread.join();
    }    
    
    cout << "Invalidaciones: " << bus.getNumInvalidations() << endl;
    cout << "Read Requests: " << bus.getNumReadRequests() << endl;
    cout << "Read Responses: " << bus.getNumReadResponses() << endl;
    cout << "Write Requests: " << bus.getNumWriteRequests() << endl;
    cout << "Write Responses: " << bus.getNumWriteResponses() << endl;

    for (int i = 0; i < numPEs; i++)
    {
        cout << "Datos transmitidos al PE" << i << ": " << bus.getDataTransmitted(i) << " bytes" << endl;
    }
    return 0;
}