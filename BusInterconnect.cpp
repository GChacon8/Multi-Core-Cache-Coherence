#include "BusInterconnect.h"
#include <unistd.h>

BusInterconnect::BusInterconnect(Ram& sharedMem, int numPEs, vector<Cache*>& caches)
	: sharedMemory(sharedMem),
	dataTransmitted(numPEs, 0),
	caches(caches),
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

future<uint64_t> BusInterconnect::enqueueRead(Cache& cache, int i, int j, int peId, int adderss)
{
	Request req;
	req.peID = peId;
	req.type = READ;
	req.address = adderss;
	// No se necesita data para un read
	future<uint64_t> fut = req.promise.get_future();
	{
		lock_guard<mutex> lock(queue_mutex);
		requestQueue.push_back(move(req));
	}
	queue_cv.notify_one();
    if(cache.get_first(i,j) == 0){
        assignMESIState(cache, i, j, EXCLUSIVE, READ);
    } else{
	    assignMESIState(cache, i, j, SHARED, READ);
    }
    //std::cout<<fut.get()<< " lol"<<std::endl;
	return fut;
}

void BusInterconnect::enqueueWrite(Cache& cache, int i, int j,int peId, int adderss, uint64_t data)
{
	Request req;
	req.peID = peId;
	req.type = WRITE;
	req.address = adderss;
	req.data = data;
	// No se necesita promise para un WRITE

	{
		lock_guard<mutex> lock(queue_mutex);
		requestQueue.push_back(move(req));
	}
	assignMESIState(cache,i, j, MODIFIED, WRITE);
	queue_cv.notify_one();
}

void BusInterconnect::alwaysWriteOnMemory(int i, int j, int peId, int address, uint64_t data)
{
	// CAMBIAR A DEQUE, PARA QUE EL WRITE-BACK OCURRA DE PRIMERO SIEMPRE
	// REVISAR LOS METODOS POP_FRONT y PUSH_FRONT
	Request req;
	req.peID = peId;
	req.type = WRITE;
	req.address = address;
	req.data = data;
	{
		lock_guard<mutex> lock(queue_mutex);
		requestQueue.push_front(move(req));
	}
	queue_cv.notify_one();
}

void BusInterconnect::notifyOtherCaches(Cache &cache, int i, int j)
{
	assignMESIState(cache, i, j, MODIFIED, WRITE);
}

void BusInterconnect::processRequests()
{
	int contador = 0;	
	while (true)
	{
		sleep(1);
		contador++;
		
		unique_lock<mutex> lock(queue_mutex);
		queue_cv.wait(lock, [this] { return !requestQueue.empty() || stopBus; });

		if (stopBus && requestQueue.empty())
		{
			break;
		}
		
		// Obtener la respuesta de la queue
		Request req = move(requestQueue.front());
		requestQueue.pop_front();
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

void BusInterconnect::assignMESIState(Cache& cache, int i, int j, MESIState newState, OperationType operationType)
{
	MESIState currentState = cache.get_state(i,j);


    cout<<"el estado actual de PE"<< cache.get_id()<< " es " << currentState <<endl;

	switch (newState)
	{
	case MODIFIED:
		if((currentState == SHARED || currentState == INVALID) && operationType == WRITE)
		{
			int address = cache.get_address(i,j);
			for (auto& other_cache : caches)
			{
				if(other_cache->get_id() != cache.get_id()) {
					if (other_cache->is_in_cache(address))
					{
						other_cache->set_state(other_cache->get_index(address).first, other_cache->get_index(address).second,INVALID);
						numInvalidations++;
						cout<<"SE INVALIDARON LOS CACHES: "<<other_cache->get_id()<<endl;
					}
				}
			}	
			cache.set_state(i, j, MODIFIED);
		} else if ((currentState == EXCLUSIVE || currentState == INVALID) && operationType == WRITE)
		{
			cache.set_state(i, j, MODIFIED);
		} else if (currentState == MODIFIED)
		{
			cache.set_state(i, j, MODIFIED);
		}

        cout<<"el nuevo estado es MODIFIED"<<endl;

		break;

	case EXCLUSIVE:
		if (currentState == INVALID && operationType == READ)
		{
			int address = cache.get_address(i,j);
            cache.set_state(i, j, EXCLUSIVE);
            cout<<"el nuevo estado es EXCLUSIVE"<<endl;
			for (auto& other_cache : caches) {
				if(other_cache->get_id() != cache.get_id()) {
                    if (other_cache->is_in_cache(cache.get_address(i, j))) {
                        cache.set_state(i, j, SHARED);
                    	if (other_cache->get_state(other_cache->get_index(address).first,other_cache->get_index(address).second) == MODIFIED) {
                    		other_cache->Writeback(other_cache->get_index(address).first,other_cache->get_index(address).second);
                    	}
                        other_cache->set_state(i, j, SHARED);
                        cout << "el nuevo estado es SHARED" << endl;
                        break;
                    }
                }
			}

		}
		break;

	case SHARED:
		if (currentState == INVALID && operationType == READ)
		{
			int address = cache.get_address(i,j);
			for (auto& other_cache : caches)
			{
				if(other_cache->get_id() != cache.get_id()) {
					if (other_cache->is_in_cache(address) && other_cache->get_state(other_cache->get_index(address).first,other_cache->get_index(address).second) != INVALID)
					{
						other_cache->set_state(other_cache->get_index(address).first,other_cache->get_index(address).second, SHARED);
						cout<<"PASARON A SHARED LOS CACHES: "<<other_cache->get_id()<<endl;
					} else if (other_cache->get_state(other_cache->get_index(address).first,other_cache->get_index(address).second) == MODIFIED )
					{
						other_cache->Writeback(other_cache->get_index(address).first,other_cache->get_index(address).second);
					}
				}
			}
			cache.set_state(i, j, SHARED);
		}
            cout<<"el nuevo estado es SHARED*"<<endl;

		break;
	case INVALID:
		cache.set_state(i, j, INVALID);
        cout<<"el nuevo estado es INVALID"<<endl;
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

uint64_t BusInterconnect::getDataTransmitted(int peId) const {
	return dataTransmitted[peId];
}
