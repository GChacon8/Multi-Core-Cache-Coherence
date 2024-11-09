#include "BusInterconnect.h"

BusInterconnect::BusInterconnect(Ram& sharedMem, int numPEs)
	: sharedMemory(sharedMem),
	dataTransmitted(numPEs, 0),
	numInvalidations(0),
	numReadRequests(0),
	numReadResponses(0),
	numWriteRequests(0),
	numWriteResponses(0) {}

uint64_t BusInterconnect::readRequest(int pe_id, int addr)
{
	lock_guard<mutex> lock(bus_mutex);
	numReadRequests++;
	uint64_t data = sharedMemory.read_mem(addr);
	numReadResponses++;
	dataTransmitted[pe_id] += sizeof(data);

	return data;
}

void BusInterconnect::writeRequest(int pe_id, int addr, uint64_t data)
{
	lock_guard<mutex> lock(bus_mutex);
	numWriteRequests++;
	sharedMemory.write_mem(addr, data);
	numWriteResponses++;
	dataTransmitted[pe_id] += sizeof(data);
}

void BusInterconnect::assignMESIState(int pe_id, int blockIndex, MESIState newState)
{
	// Obtener el estado del cache y guardaro en currentCache
	MESIState currentState; // Cambiar a MESIState currentState = cache.getState(blockindex); o algo asi

	switch (newState)
	{
	case MODIFIED:
		if (currentState == SHARED || currentState == EXCLUSIVE)
		{
			// Invalidar otras caches que no sean pe_id
			// Cambiar el estado a modified de ese bloque
			numInvalidations++;
		}
		break;

	case EXCLUSIVE:
		if (currentState == INVALID)
		{
			// Cambiar el estado a E
		}
		break;

	case SHARED:
		if (currentState == EXCLUSIVE)
		{
			// Cambiar estado a S
		}
		break;
	case INVALID:
		// Se mantiene en I
		break;
	default:
		break;
	}
}

void BusInterconnect::registerInvalidation(int pe_id, int regIndex)
{
	/*
	* Invalidar las otras caches aqui
	* algo como
	* para cada PE:
	*	si  PE.id != pe_id
	*		PE.setState(pe_id, regIndex, Invalid)
	*/
}

int BusInterconnect::getNumInvalidations() const { return numInvalidations; }

int BusInterconnect::getNumReadRequests() const { return numReadRequests; }

int BusInterconnect::getNumReadResponses() const { return numReadResponses; }

int BusInterconnect::getNumWriteRequests() const { return numWriteRequests; }

int BusInterconnect::getNumWriteResponses() const{ return numWriteResponses; }

uint64_t BusInterconnect::getDataTransmitted(int peId) const { return dataTransmitted[peId]; }
