#include "cache.h"
#include <iostream>
#include <queue>
#include <future>

using namespace std;

// Constructor de la clase Cache
Cache::Cache(int cache_id, BusInterconnect& bus) : id(cache_id), bus(bus), miss_count(0), inv_count(0) {
    // Inicializar todos los bloques de la caché
    for (int i = 0; i < 8; i++) {
        valid[i] = false;
        dirty[i] = false;
        data[i] = 0;
        addr[i] = 0;
        first[i] = 0;
        state[i] = INVALID;  // Asegúrate de inicializar el estado en INVALID
    }
}

// Método para obtener los datos de un bloque
uint64_t Cache::get_data(int block_num) {
    return data[block_num];
}

// Método para escribir un valor en un bloque
void Cache::set_data(int block_num, int pos, uint64_t value) {
    data[block_num] = value;
}

// Método para escribir un valor en la caché
void Cache::write(uint8_t address, uint64_t value) {
    uint8_t tag = address;

    // Buscar si ya existe el bloque (para actualizar si hay cache hit)
    for (int index = 0; index < 8; ++index) {
        if (valid[index] && addr[index] == tag) { // Cache hit
            data[index] = value;
            cout << "Cache hit (Cache " << id << "): valor leído en el índice [" << index << "].\n";
            return;
        }
    }

    // Buscar un bloque inválido para escribir el valor (cache miss)
    for (int index = 0; index < 8; ++index) {
        if (!valid[index]) { // Encontrar un bloque inválido
            // Escribir el valor en el bloque inválido
            data[index] = read_memory(tag);
            data[index] = value;
            addr[index] = tag;
            valid[index] = true;
            dirty[index] = true;
            miss_count++;
            fifo_queue.push(index);  // Añadir el índice a la cola FIFO
            cout << "Cache miss (Cache " << id << "): dato escrito en el índice [" << index << "].\n";
            return;
        }
    }

    // Si no hay bloques inválidos (cache llena), reemplazar un bloque (usamos FIFO)
    int old_index = fifo_queue.front();  // Obtener el índice del bloque más antiguo
    fifo_queue.pop();  // Eliminar el bloque de la cola FIFO

    // Si el bloque antiguo está "dirty", hacer write-back a memoria
    if (dirty[old_index]) {
        cout << "Write-back (Cache " << id << "): escribiendo datos del índice [" << old_index << "] a memoria.\n";
        write_memory(old_index);
    }

    // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
    if (valid[old_index]) {
        inv_count++;
        cout << "Invalidación (Cache " << id << "): bloque en el índice [" << old_index << "] invalidado.\n";
    }

    // Reemplazar el bloque con el nuevo valor
    data[old_index] = value;
    addr[old_index] = tag;
    valid[old_index] = true;
    dirty[old_index] = true;
    fifo_queue.push(old_index);  // Agregar la nueva posición al final de la cola
}

// Método para leer un valor de la caché
uint64_t Cache::read(uint8_t address) {
    uint8_t tag = address;

    // Buscar si ya existe el bloque (para un cache hit)
    for (int index = 0; index < 8; ++index) {
        if (valid[index] && addr[index] == tag) { // Cache hit
            cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
            return data[index];
        }
    }

    // Buscar un bloque inválido para cargar el valor desde memoria (cache miss)
    for (int index = 0; index < 8; ++index) {
        if (!valid[index]) { // Encontrar un bloque inválido
            addr[index] = tag;
            data[index] = read_memory(address); // Leer el valor de memoria
            valid[index] = true;
            first[index]++;
            miss_count++;
            fifo_queue.push(index);  // Añadir el índice a la cola FIFO
            cout << "Cache miss (Cache " << id << "): dato guardado en el índice [" << index << "].\n";
            return data[index];
        }
    }

    // Si no hay bloques inválidos (cache llena), reemplazar un bloque (usamos FIFO)
    int old_index = fifo_queue.front();  // Obtener el índice del bloque más antiguo
    fifo_queue.pop();  // Eliminar el bloque de la cola FIFO

    // Si el bloque antiguo está "dirty", hacer write-back a memoria
    if (dirty[old_index]) {
        cout << "Write-back (Cache " << id << "): escribiendo datos del índice [" << old_index << "] a memoria.\n";
        write_memory(old_index);
    }

    // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
    if (valid[old_index]) {
        inv_count++;
        cout << "Invalidación (Cache " << id << "): bloque en el índice [" << old_index << "] invalidado.\n";
    }

    // Reemplazar el bloque con el nuevo valor
    data[old_index] = read_memory(address);  // Leer el nuevo valor de memoria
    addr[old_index] = tag;
    valid[old_index] = true;
    dirty[old_index] = false;
    fifo_queue.push(old_index);  // Agregar la nueva posición al final de la cola

    return data[old_index];
}

// Métodos de acceso a los contadores y el estado
int Cache::get_id() const {
    return id;
}

int Cache::get_miss_count() const {
    return miss_count;
}

int Cache::get_invalidation_count() const {
    return inv_count;
}

MESIState Cache::get_state(int block_num) {
    return state[block_num];
}

void Cache::set_state(int block_num, MESIState mesiState) {
    state[block_num] = mesiState;
}

// Métodos para interactuar con la memoria
uint64_t Cache::read_memory(uint8_t address) {
    future<uint64_t> load_data = bus.enqueueRead(*this, get_index(address), id, address);
    return load_data.get(); // Leer el valor de memoria
}

void Cache::write_memory(int block_num) {
    bus.enqueueWrite(*this, block_num, id, addr[block_num], data[block_num]);
}

// Métodos de utilidad
uint8_t Cache::get_address(int index) {
    return addr[index];
}

int Cache::get_index(uint8_t address) {
    for (int index = 0; index < 8; ++index) {
        if (addr[index] == address) { // Cache hit
            cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
            return index;
        }
    }
    return -1;  // No encontrado
}

bool Cache::is_in_cache(uint8_t address) {
    for (int index = 0; index < 8; ++index) {
        if (addr[index] == address) { // Cache hit
            cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
            return true;
        }
    }
    return false;
}

int Cache::get_first(int index) {
    return first[index];
}
