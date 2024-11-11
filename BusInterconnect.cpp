#include "BusInterconnect.h"

BusInterconnect::BusInterconnect(Ram& sharedMem, int numPEs)
	: sharedMemory(sharedMem),
	dataTransmitted(numPEs, 0),
	numInvalidations(0),
	numReadRequests(0),
	numReadResponses(0),
	numWriteRequests(0),
	numWriteResponses(0) {}

BusInterconnect::~BusInterconnect()
{
	{
		lock_guard<mutex> lock(queue_mutex);
		stopBus = true;
	}
	queue_cv.notify_all();
	if (busThread.joinable())
	{
		busThread.join();
	}
}

future<uint64_t> BusInterconnect::enqueueRead(int peId, int adderss)
{
	Request req;
	req.peID = peId;
	req.type = READ;
	req.address = adderss;
	// No se necesita data para un read
	future<uint64_t> fut = req.promise.get_future();
	{
		lock_guard<mutex> lock(queue_mutex);
		requestQueue.push(move(req));
	}
	queue_cv.notify_one();
	return fut;
}

void BusInterconnect::enqueueWrite(int peId, int adderss, uint64_t data)
{
	Request req;
	req.peID = peId;
	req.type = WRITE;
	req.address = adderss;
	req.data = data;
	// No se necesita promise para un WRITE

	{
		lock_guard<mutex> lock(queue_mutex);
		requestQueue.push(move(req));
	}
	queue_cv.notify_one();
}

void BusInterconnect::processRequests()
{
	while (true)
	{
		unique_lock<mutex> lock(queue_mutex);
		queue_cv.wait(lock, [this] { return !requestQueue.empty() || stopBus; });

		if (stopBus && requestQueue.empty())
		{
			break;
		}

		// Obtener la respuesta de la queue
		Request req = move(requestQueue.front());
		requestQueue.pop();
		lock.unlock();

		// Procesa la solicitud
		if (req.type == READ)
		{
			{
				lock_guard<mutex> busLock(bus_mutex);
				numReadRequests++;
				uint64_t data = sharedMemory.read_mem(req.address);
				numReadResponses++;
				dataTransmitted[req.peID] += sizeof(data);
				req.promise.set_value(data);
			}
		}
		else if(req.type == WRITE)
		{
			{
				lock_guard<mutex> busLock(bus_mutex);
				numWriteRequests++;
				sharedMemory.write_mem(req.address, req.data);
				numWriteResponses++;
				dataTransmitted[req.peID] += sizeof(req.data);
			}
		}
	}
}

void BusInterconnect::assignMESIState(Cache& cache, int blockIndex, MESIState newState)
{
	MESIState currentState = cache.getState(blockIndex);

	switch (newState)
	{
	case MODIFIED:
		if (currentState == SHARED || currentState == EXCLUSIVE)
		{
			cache.invalidateOtherCaches(blockIndex);
			cache.setState(blockIndex, MODIFIED);
		}
		break;
	
	case EXCLUSIVE:
		if (currentState == INVALID)
		{
			cache.setState(blockIndex, EXCLUSIVE);
		}
		break;

	case SHARED:
		if (currentState == EXCLUSIVE)
		{
			cache.setState(blockIndex, SHARED);
		}
		break;

	case INVALID:
		cache.setState(blockIndex, INVALID);
		break;
	default:
		break;
	}
}

void BusInterconnect::registerInvalidation(int peId)
{
	lock_guard<mutex> lock(bus_mutex);
	numInvalidations++;
}

int BusInterconnect::getNumInvalidations() const { return numInvalidations; }

int BusInterconnect::getNumReadRequests() const { return numReadRequests; }

int BusInterconnect::getNumReadResponses() const { return numReadResponses; }

int BusInterconnect::getNumWriteRequests() const { return numWriteRequests; }

int BusInterconnect::getNumWriteResponses() const{ return numWriteResponses; }
