#pragma once
#ifndef BUS_INTERCONNECT_H
#define	BUS_INTERCONNECT_H

#include <vector>
#include <mutex>
#include <atomic>
#include <cstdint>
#include "RAM.cpp"
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

class BusInterconnect {
public:
	BusInterconnect(Ram& sharedMem, int numPEs);

	uint64_t readRequest(int pe_id, int addr);
	void writeRequest(int pe_id, int addr, uint64_t data);

	// Metodo para asignar mesi, hace referencia Cache& cache, primer parametro
	void assignMESIState(int pe_id, int blockIndex, MESIState newState);
	void registerInvalidation(int pe_id, int regIndex);

	// agregar esquemas de arbitraje

	int getNumInvalidations() const;
	int getNumReadRequests() const;
	int getNumReadResponses() const;
	int getNumWriteRequests() const;
	int getNumWriteResponses() const;
	uint64_t getDataTransmitted(int peId) const;

private:
	Ram& sharedMemory;
	vector<uint64_t> dataTransmitted;
	mutex bus_mutex;

	atomic<int> numInvalidations;
	atomic<int> numReadRequests;
	atomic<int> numReadResponses;
	atomic<int> numWriteRequests;
	atomic<int> numWriteResponses;
};

#endif // !BUS_INTERCONNECT_H
