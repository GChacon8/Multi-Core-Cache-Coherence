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
#include "RAM.cpp"
#include "cache.cpp"
// #include "Cache.h"
// Asumo que asi se llama la libreria, tambi�n puede ser que est� anidada dentro de los PEs

using namespace std;

class Ram;

// class Cache;

enum MESIState {
	MODIFIED,
	EXCLUSIVE,
	SHARED,
	INVALID
};

enum OperationType {
	READ,
	WRITE
};

struct Request
{
	int peID;
	OperationType type;
	int address;
	uint64_t data;
	promise<uint64_t> promise;
};

class BusInterconnect {
public:
	// Constructor
	BusInterconnect(Ram& sharedMem, int numPEs);
	~BusInterconnect();

	// Metodos lectura y escritura
	future<uint64_t> enqueueRead(int peId, int addr);
	void enqueueWrite(int peId, int address, uint64_t data);

	// Metodo para asignar mesi, hace referencia Cache& cache, primer parametro
	void assignMESIState(Cache& cache, int blockIndex, MESIState newState);

	// Registrar invalidacion
	void registerInvalidation(int peId);

	int getNumInvalidations() const;
	int getNumReadRequests() const;
	int getNumReadResponses() const;
	int getNumWriteRequests() const;
	int getNumWriteResponses() const;
	uint64_t getDataTransmitted(int peId) const;

private:
	void processRequests();

	Ram& sharedMemory;
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
	bool stopBus;
};

#endif // !BUS_INTERCONNECT_H
