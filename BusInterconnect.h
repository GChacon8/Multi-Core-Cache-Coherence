#pragma once
#ifndef BUS_INTERCONNECT_H
#define	BUS_INTERCONNECT_H

#include <vector>
#include <mutex>
#include <atomic>
#include <cstdint>
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

class BusInterconnect {
public:
	BusInterconnect(Ram& sharedMem, vector<Cache*>& caches);

	uint64_t readRequest(Cache& cache, int addr);
	void writeRequest(Cache& cache, int addr, uint64_t data);

	// Metodo para asignar mesi, hace referencia Cache& cache, primer parametro
	void assignMESIState(Cache& cache, int blockIndex, MESIState newState);

	// agregar esquemas de arbitraje

	int getNumInvalidations() const;
	int getNumReadRequests() const;
	int getNumReadResponses() const;
	int getNumWriteRequests() const;
	int getNumWriteResponses() const;

private:
	Ram& sharedMemory;
	vector<Cache*> caches;
	mutex bus_mutex;

	atomic<int> numInvalidations;
	atomic<int> numReadRequests;
	atomic<int> numReadResponses;
	atomic<int> numWriteRequests;
	atomic<int> numWriteResponses;

	void invalidateOtherCaches(Cache& requestingCache, int blockIndex);
	MESIState getCurrentMESIState(int blockIndex);
};

#endif // !BUS_INTERCONNECT_H
