#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <queue>
#include <future>
#include "BusInterconnect.h"

// Clase que representa la memoria caché
class BusInterconnect;

class Cache {
private:
    int id;                         // Identificador de la caché
    uint64_t data[8];               // Datos almacenados en caché
    uint8_t addr[8];                // Direcciones de memoria almacenadas en caché
    MESIState state[8];             // Estado MESI de cada bloque
    bool valid[8];                  // Indica si el bloque es válido
    bool dirty[8];                  // Indica si el bloque es sucio
    int first[8];                   // Indicador de la primera vez que se usa un bloque
    std::queue<int> fifo_queue;     // Cola FIFO para el reemplazo de bloques
    int miss_count;                 // Contador de fallos de caché
    int inv_count;                  // Contador de invalidaciones
    BusInterconnect& bus;           // Referencia a la interconexión de bus

public:
    // Constructor
    Cache(int cache_id, BusInterconnect& bus);

    // Métodos de acceso a los atributos
    uint64_t get_data(int block_num);
    void set_data(int block_num, int pos, uint64_t value);
    uint64_t read(uint8_t address);
    void write(uint8_t address, uint64_t value);
    int get_id() const;
    int get_miss_count() const;
    int get_invalidation_count() const;
    MESIState get_state(int block_num);
    void set_state(int block_num, MESIState mesiState);

    // Métodos para interactuar con la memoria
    uint64_t read_memory(uint8_t address);
    void write_memory(int block_num);

    // Métodos de utilidad
    uint8_t get_address(int index);
    int get_index(uint8_t address);
    bool is_in_cache(uint8_t address);
    int get_first(int index);

    void Writeback(int blocknum);
};

#endif // CACHE_H
