#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <queue>
#include <future>
#include "BusInterconnect.h"
#include <utility>  // Para std::pair

using namespace std;

// Clase que representa la memoria caché
class BusInterconnect;

class Cache {
private:
    int id;                         // Identificador de la caché
    uint64_t data[8][4];               // Datos almacenados en caché
    uint8_t addr[8][4];                // Direcciones de memoria almacenadas en caché
    MESIState state[8][4];             // Estado MESI de cada bloque
    bool valid[8][4];                  // Indica si el bloque es válido
    bool dirty[8][4];                  // Indica si el bloque es sucio
    int first[8][4];                   // Indicador de la primera vez que se usa un bloque
    std::queue<std::pair<int, int>> fifo_queue;    // Cola FIFO para el reemplazo de bloques
    int miss_count;                 // Contador de fallos de caché
    int inv_count;                  // Contador de invalidaciones
    BusInterconnect& bus;           // Referencia a la interconexión de bus

public:
    // Constructor
    Cache(int cache_id, BusInterconnect& bus);

    // Métodos de acceso a los atributos
    void set_data(int block_num, int pos, uint64_t value);
    uint64_t read(uint8_t address);
    void write(uint8_t address, uint64_t value);
    int get_id() const;
    int get_miss_count() const;
    int get_invalidation_count() const;

    // Métodos para interactuar con la memoria
    uint64_t read_memory(uint8_t address);

    // Métodos de utilidad
    pair<int, int> get_index(uint8_t address);
    bool is_in_cache(uint8_t address);


    uint64_t get_data(int row, int col);

    MESIState get_state(int i, int j);

    uint8_t get_address(int i, int j);

    void Writeback(int i, int j);

    void write_memory(int i, int j);

    void set_state(int i, int j, MESIState mesiState);

    int get_first(int i, int j);

    void displayStats();
};

#endif // CACHE_H
