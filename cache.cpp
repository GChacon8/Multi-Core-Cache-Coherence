#include <iostream>
#include <bitset>  // Librería para bitset
#include <cstdint> // For uint64_t
#include "BusInterconnect.h"
#include <queue>
#include <future>
using namespace std;

class Cache {
private:
    // Atributos
    int id;

    uint64_t data[32];
    uint8_t addr[32];
    MESIState state[32];
    bool valid[32];
    bool dirty[32];
    bool first[32];
    queue<int> fifo_queue; // Ahora solo necesitamos un entero para el índice en lugar de un par
    int miss_count; // Sigue igual
    int inv_count;  // Sigue igual
    BusInterconnect& bus;

public:

    Cache(int cache_id, BusInterconnect& bus):id(cache_id),bus(bus) {

        // definir todos los bits validos como 0
        for (int i = 0; i < 32; i++) {
            valid[i] = false;
            dirty[i] = false;
            data[i] = 0;
            addr[i] = 0;
        }

    }
    
    // Métodos para obtener los atributos
    uint64_t get_data(int block_num) {
        return data[block_num];
    }

    void set_data(int block_num, int pos, uint64_t value) {
        data[block_num] = value;
    }

    void write(uint8_t address, uint64_t value) {
        uint8_t tag = address;

        // Buscar si ya existe el bloque (para actualizar si hay cache hit)
        for (int index = 0; index < 32; ++index) {
            if (valid[index] && addr[index] == tag) { // Cache hit
                data[index] = value;
                cout << "Cache hit (Cache " << id << "): valor leído en el índice [" << index << "].\n";
                return;
            }
        }

        // Buscar un bloque inválido para escribir el valor (cache miss)
        for (int index = 0; index < 32; ++index) {
            if (!valid[index]) { // Encontrar un bloque inválido
                // Escribir el valor en el bloque inválido
                data[index]= read_memory(tag);
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

    uint64_t read(uint8_t address) {
        uint8_t tag = address;
        uint64_t value;

        // Buscar si ya existe el bloque (para un cache hit)
        for (int index = 0; index < 32; ++index) {
            if (valid[index] && addr[index] == tag) { // Cache hit
                cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
                return data[index];
            }
        }

        // Buscar un bloque inválido para cargar el valor desde memoria (cache miss)
        for (int index = 0; index < 32; ++index) {
            if (!valid[index]) { // Encontrar un bloque inválido
                data[index] = read_memory(address); // Leer el valor de memoria
                addr[index] = tag;
                valid[index] = true;
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


    int get_id() const{
        return id;
    }

    // Métodos para obtener los contadores de eventos
    int get_miss_count() const {
        return miss_count;
    }

    int get_invalidation_count() const {
        return inv_count;
    }

    MESIState get_state(int block_num) {
        return state[block_num];
    }

    void set_state(int block_num, MESIState mesiState) {
        state[block_num] = mesiState;
    }

    uint64_t read_memory(uint8_t address){
        future<uint64_t> load_data = bus.enqueueRead(*this, this->get_index(address),id, address);
        return load_data.get(); // Leer el valor de memoria
    }

    void write_memory(int block_num){
        bus.enqueueWrite(*this, block_num, id, addr[block_num], data[block_num]);
    }

    uint8_t get_address(int index){
        return addr[index];
    };

    int get_index(uint8_t address){
        for (int index = 0; index < 32; ++index) {
            if (addr[index] == address) { // Cache hit
                cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
                return index;
            }
        }

    }

    bool is_in_cache(uint8_t address){
        for (int index = 0; index < 32; ++index) {
            if (addr[index] == address) { // Cache hit
                cout << "Cache hit (Cache " << id << "): leyendo dato en el índice [" << index << "].\n";
                return true;
            }
        }
        return false;

    }
};
