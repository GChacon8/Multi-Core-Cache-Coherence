#include "cache.h"
#include <iostream>
#include <queue>
#include <future>

using namespace std;

// Constructor de la clase Cache
Cache::Cache(int cache_id, BusInterconnect& bus) : id(cache_id), bus(bus), miss_count(0), inv_count(0), hit_count(0) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            valid[i][j] = false;
            dirty[i][j] = false;
            data[i][j] = 0;
            addr[i][j] = 0;
            first[i][j] = 0;
            state[i][j] = INVALID;
        }
    }
}

// Método para obtener los datos de un bloque
uint64_t Cache::get_data(int row, int col) {
    return data[row][col];
}



// Método para escribir un valor en la caché
void Cache::write(uint8_t address, uint64_t value) {
    uint8_t tag = address;

    // Buscar si ya existe el bloque (para actualizar si hay cache hit)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (valid[i][j] && addr[i][j] == tag) { // Cache hit
                if (state[i][j] == SHARED || state[i][j] == INVALID) {
                    bus.notifyOtherCaches(*this, i, j); // Cambiado para la matriz 8x4
                }
                if(state[i][j] == EXCLUSIVE){
                    state[i][j]=MODIFIED;
                }
                data[i][j] = value;
                cout << "Cache hit (Cache " << id << ") Memoria( " << static_cast<unsigned int>(tag)
                     << "): valor leído en el índice [" << i << ", " << j << "].\n";
                return;
            }
        }
    }

    // Buscar un bloque inválido para escribir el valor (cache miss)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!valid[i][j]) { // Encontrar un bloque inválido
                // Escribir el valor en el bloque inválido
                data[i][j] = value;
                addr[i][j] = tag;
                valid[i][j] = true;
                dirty[i][j] = true;
                miss_count++;
                bus.notifyOtherCaches(*this, i, j); // Cambiado para la matriz 8x4
                fifo_queue.push({i, j});  // Añadir el índice a la cola FIFO como par {i, j}
                cout << "Cache miss (Cache " << id << ") Memoria( " << static_cast<unsigned int>(tag)
                     << "): dato escrito en el índice [" << i << ", " << j << "].\n";
                return;
            }
        }
    }

    // Si no hay bloques inválidos (cache llena), reemplazar un bloque (usamos FIFO)
    auto [old_i, old_j] = fifo_queue.front();  // Obtener el índice del bloque más antiguo como par {i, j}
    fifo_queue.pop();  // Eliminar el bloque de la cola FIFO

    // Si el bloque antiguo está "dirty", hacer write-back a memoria
    if (state[old_i][old_j] == MODIFIED) {
        cout << "Write-back (Cache " << id << "): escribiendo datos del índice [" << old_i << ", " << old_j << "] a memoria.\n";
        Writeback(old_i, old_j);
    }

    // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
    if (valid[old_i][old_j]) {
        inv_count++;
        cout << "Invalidación (Cache " << id << "): bloque en el índice [" << old_i << ", " << old_j << "] invalidado.\n";
    }

    // Reemplazar el bloque con el nuevo valor
    data[old_i][old_j] = value;
    addr[old_i][old_j] = tag;
    valid[old_i][old_j] = true;
    dirty[old_i][old_j] = true;
    fifo_queue.push({old_i, old_j});  // Agregar la nueva posición al final de la cola como par {i, j}
    bus.notifyOtherCaches(*this, old_i, old_j);  // Cambiado para la matriz 8x4
}


// Método para leer un valor de la caché
uint64_t Cache::read(uint8_t address) {
    uint8_t tag = address;

    // Buscar si ya existe el bloque (para un cache hit)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (valid[i][j] && addr[i][j] == tag) { // Cache hit
                cout << "Cache hit (Cache " << id << ") Memoria( " << static_cast<unsigned int>(tag)
                     << "): leyendo dato en el índice [" << i << ", " << j << "].\n";
                hit_count++;
                if (state[i][j] != EXCLUSIVE) {
                    bus.assignMESIState(*this, i, j, SHARED, READ);  // Modificado para aceptar {i, j}
                }
                return data[i][j];
            }
        }
    }

    // Buscar un bloque inválido para cargar el valor desde memoria (cache miss)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!valid[i][j]) { // Encontrar un bloque inválido
                addr[i][j] = tag;
                data[i][j] = read_memory(address);  // Leer el valor de memoria
                valid[i][j] = true;
                first[i][j]++;
                miss_count++;
                fifo_queue.push({i, j});  // Añadir el índice a la cola FIFO como par {i, j}
                cout << "Cache miss (Cache " << id << ") Memoria( " << static_cast<unsigned int>(tag)
                     << "): dato guardado en el índice [" << i << ", " << j << "].\n";
                return data[i][j];
            }
        }
    }

    // Si no hay bloques inválidos (cache llena), reemplazar un bloque (usamos FIFO)
    auto [old_i, old_j] = fifo_queue.front();  // Obtener el índice del bloque más antiguo como par {i, j}
    fifo_queue.pop();  // Eliminar el bloque de la cola FIFO

    // Si el bloque antiguo está "dirty", hacer write-back a memoria
    if (state[old_i][old_j] == MODIFIED) {
        cout << "Write-back (Cache " << id << "): escribiendo datos del índice [" << old_i << ", " << old_j << "] a memoria.\n";
        Writeback(old_i, old_j);
    }

    // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
    if (valid[old_i][old_j]) {
        inv_count++;
        cout << "Invalidación (Cache " << id << "): bloque en el índice [" << old_i << ", " << old_j << "] invalidado.\n";
    }

    // Reemplazar el bloque con el nuevo valor
    data[old_i][old_j] = read_memory(address);  // Leer el nuevo valor de memoria
    addr[old_i][old_j] = tag;
    valid[old_i][old_j] = true;
    dirty[old_i][old_j] = false;
    fifo_queue.push({old_i, old_j});  // Agregar la nueva posición al final de la cola como par {i, j}

    // VERIFICAR SI EL NUEVO BLOQUE ESTÁ COMPARTIDO EN OTRA CACHE
    // (Este proceso se debe implementar aquí si es necesario)

    return data[old_i][old_j];
}


// Métodos de acceso a los contadores y el estado
int Cache::get_id() const {
    return id;
}

// Métodos de utilidad
uint8_t Cache::get_address(int i, int j) {
    return addr[i][j];
}

int Cache::get_invalidation_count() const {
    return inv_count;
}

MESIState Cache::get_state(int i, int j) {
    return state[i][j];
}

// Método para escribir un valor en un bloque
void Cache::set_data(int row, int col, uint64_t value) {
    data[row][col] = value;
}

void Cache::set_state(int i, int j, MESIState mesiState) {
    Last_oldState = get_state(i,j);
    Last_newState = mesiState;
    Last_addr = get_address(i,j);

    state[i][j] = mesiState;
}

// Métodos para interactuar con la memoria
uint64_t Cache::read_memory(uint8_t address) {
    future<uint64_t> load_data = bus.enqueueRead(*this, get_index(address).first, get_index(address).second, id, address);
    return load_data.get(); // Leer el valor de memoria
}

void Cache::write_memory(int i,int j) {
    bus.enqueueWrite(*this, i, j, id, addr[i][j], data[i][j]);
}

void Cache::Writeback(int i, int j){
    bus.alwaysWriteOnMemory(i, j, id, addr[i][j], data[i][j]);
}


std::pair<int, int> Cache::get_index(uint8_t address) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (addr[i][j] == address) { // Cache hit
                return {i, j};
            }
        }
    }
    return {-1, -1};  // No encontrado
}

bool Cache::is_in_cache(uint8_t address) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (addr[i][j] == address) { // Cache hit
                cout << "address "<< address <<" esta en Cache " << id << "): leyendo dato en el indice [" << i << ", " << j << "].\n";
                return true;
            }
        }
    }
    return false;
}


int Cache::get_first(int i,int j) {
    return first[i][j];
}

void Cache::displayStats() {
    cout << "Datos de cache del PE" << id << ": " << endl;
    cout << "Misses: " << miss_count << endl;
    cout << "Invalidaciones: " << inv_count << endl;
}

int Cache::getID(){
    return id;
}

int Cache::get_miss_count(){
    return miss_count;
}

int Cache::get_inv_count(){
    return inv_count;
}

int Cache::get_hit_count(){
    return hit_count;
}


uint8_t Cache::get_last_addr(){
    return Last_addr;
}

MESIState Cache::get_last_oldState(){
    return Last_oldState;
}

MESIState Cache::get_last_newState(){
    return Last_newState;
}
