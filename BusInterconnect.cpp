#include "BusInterconnect.h"

BusInterconnect::BusInterconnect(Ram& sharedMem, vector<Cache*>& caches)
	: sharedMemory(sharedMem),
	caches(caches),
	numInvalidations(0),
	numReadRequests(0),
	numReadResponses(0),
	numWriteRequests(0),
	numWriteResponses(0) {}

uint64_t BusInterconnect::readRequest(Cache& cache, int addr)
{
	lock_guard<mutex> lock(bus_mutex);
	numReadRequests++;

	int blockIndex = addr / cache.getBlockSize(); // Se necesita obtener el tamano del bloque
	MESIState currentState = getCurrentMESIState(blockIndex);

	if (currentState == INVALID)
	{
		assignMESIState(cache, blockIndex, SHARED); // Camiba a S si estaba en I
		return sharedMemory.read_mem(addr);
	}
	numReadResponses++;
	return cache.get_data(blockIndex, addr);
}

void BusInterconnect::writeRequest(Cache& cache, int addr, uint64_t data)
{
	lock_guard<mutex> lock(bus_mutex);
	numWriteRequests++;

	int blockIndex = addr / cache.getBlockSize(); // Se necesita obtener el tamano del bloque
	MESIState currentState = getCurrentMESIState(blockIndex);

	if (currentState == SHARED || currentState == INVALID)
	{
		invalidateOtherCaches(cache, blockIndex); // Invalidar las otras cache según el bloque
		assignMESIState(cache, blockIndex, MODIFIED); // Cambiar a M
	}
	numWriteResponses++;
	cache.write(addr, data);
}

void BusInterconnect::assignMESIState(Cache& cache, int blockIndex, MESIState newState)
{
	cache.setState(blockIndex, newState); // Se necesita fijar el nuevo state
	if (newState == INVALID)
	{
		numInvalidations++;
	}
}

int BusInterconnect::getNumInvalidations() const { return numInvalidations; }

int BusInterconnect::getNumReadRequests() const { return numReadRequests; }

int BusInterconnect::getNumReadResponses() const { return numReadResponses; }

int BusInterconnect::getNumWriteRequests() const { return numWriteRequests; }

int BusInterconnect::getNumWriteResponses() const{ return numWriteResponses; }

void BusInterconnect::invalidateOtherCaches(Cache &requestingCache, int blockIndex)
{
	for (Cache* cache : caches)
	{
		if (cache != &requestingCache && cache->getState(blockIndex) != INVALID) // Obtener el state
		{
			cache->setState(blockIndex, INVALID); // Fijar el state
			numInvalidations++;
		}
	}
}

MESIState BusInterconnect::getCurrentMESIState(int blockIndex)
{
    for (Cache* cache : caches)
	{
		if(cache->getState(blockIndex) != INVALID)	// Obtener el STATE
		{
			return cache->getState(blockIndex);		// Obtener el STATE
		}
	}
	return INVALID;	
}
