#pragma once
#ifndef BUS_INTERCONNECT_H
#define	BUS_INTERCONNECT_H
#include <vector>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <stdexcept>
#include <iostream>
#include "Ram.h"

// Enumeración para los estados MESI
enum MESIState {
    MODIFIED,
    EXCLUSIVE,
    SHARED,
    INVALID
};

// Enumeración para los tipos de operaciones
enum OperationType {
    READ,
    WRITE
};

// Estructura para representar una solicitud de memoria
struct Request {
    int peID;                      // Identificador del procesador
    OperationType type;            // Tipo de operación (lectura o escritura)
    int address;                   // Dirección de memoria
    uint64_t data;                 // Datos de la solicitud
    std::promise<uint64_t> promise;  // Promesa asociada a la operación
};

#include "cache.h"
using namespace std;

class Cache;
class Ram;

class BusInterconnect {
public:
	// Constructor
	BusInterconnect(Ram& sharedMem, int numPEs, vector<Cache*>& caches);
	~BusInterconnect();

	// Metodos lectura y escritura
	future<uint64_t> enqueueRead(Cache& cache, int blockIndex, int peId, int adderss);
	void enqueueWrite(Cache& cache, int blockIndex, int peId, int address, uint64_t data);
	void alwaysWriteOnMemory(int blockIndex, int peId, int address, uint64_t data);

	void notifyOtherCaches(Cache& cache, int blockIndex);

	// Metodo para asignar mesi, hace referencia Cache& cache, primer parametro
	void assignMESIState(Cache& cache, int blockIndex, MESIState newstate, OperationType operationType);
	//void assignMESIState(Cache& cache, int blockIndex, MESIState newState, OperationType operationType);

	// Registrar invalidacion
	void registerInvalidation(int peId);

	int getNumInvalidations() const;
	int getNumReadRequests() const;
	int getNumReadResponses() const;
	int getNumWriteRequests() const;
	int getNumWriteResponses() const;
	uint64_t getDataTransmitted(int peId) const;
    void processRequests();
	bool stopBus;
	
private:
	Ram& sharedMemory;
	vector<Cache*>& caches;
	vector<uint64_t> dataTransmitted;
	mutex bus_mutex;

	// Contadores de eventos
	atomic<int> numInvalidations;
	atomic<int> numReadRequests;
	atomic<int> numReadResponses;
	atomic<int> numWriteRequests;
	atomic<int> numWriteResponses;

	// Cola de solicitudes
	queue<Request> requestQueue;
	mutex queue_mutex;
	condition_variable queue_cv;

	// Hilo del bus
	thread busThread;
	
};

#endif // !BUS_INTERCONNECT_H
