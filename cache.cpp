#include <iostream>
#include <bitset>  // Librería para bitset
#include <cstdint> // For uint64_t
#include <queue>
using namespace std;

class Cache {
private:
    // Atributos
    int id;
    
    uint64_t data[4][8];
    uint8_t addr[4][8];
    bitset<2> state[4][8];
    int miss_count; //cambio porque quiero contar la cantidad total de miss
    int inv_count; //contador de invalidaciones
    bool valid[4][8]; //indica si la dirección de memoria es valida
    bool dirty[4][8]; //indica si la direccion de memoria esta sucia (write_back)
    queue<pair<int, int>> fifo_queue; // Cola FIFO para el reemplazo

public:

    Cache(int cache_id):id(cache_id) {

        // definir todos los bits validos como 0
        for (int i=0; i<4 ; i++){
            for (int j=0; j<8; j++){
                valid[i][j]=false;
                dirty[i][j]= false;
                data[i][j]=0;
                addr[i][j]=0;
            }
        }
    }
    
    // Métodos para obtener los atributos
    uint64_t get_data(int block_num, int pos) {
        return data[pos][block_num];
    }

    void set_data(int block_num, int pos, uint64_t value) {
        data[pos][block_num] = value;
    }

    void write(uint8_t address, uint64_t value){
        uint8_t tag=address;
    
     // Buscar si ya existe el bloque (para actualizar si hay cache hit)
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (valid[i][j] && addr[i][j] == tag ) { // Cache hit
                    data[i][j] = value;
                    cout << "Cache hit (Cache " << id << "): valor leido de [" << i << "][" << j << "].\n";
                    return;
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (!valid[i][j]) { // Encontrar un bloque inválido
                    data[i][j] = value;
                    addr[i][j] = tag;
                    valid[i][j] = true;
                    dirty[i][j] = true;
                    miss_count++;
                    fifo_queue.push({i, j});
                    cout << "Cache miss (Cache " << id << "): dato escrito en bloque [" << i << "][" << j << "].\n";
                    return;
                }
            }
        }

        // Si no hay bloques inválidos (cache llena), reemplaza un bloque (usamos fifo)
        pair<int, int> old_position = fifo_queue.front();  // Obtener el bloque más antiguo
        fifo_queue.pop();  // Eliminar el bloque de la cola FIFO
        int i = old_position.first;
        int j = old_position.second;

        // Si el bloque antiguo está "dirty", hacer write-back a memoria
        if (dirty[i][j]) {
            cout << "Write-back (Cache " << id << "): escribiendo datos del bloque [" << i << "][" << j << "] a memoria.\n";
            // Aquí iría la lógica para escribir a memoria
        }

        // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
        if (valid[i][j]) {
            inv_count++;
            cout << "Invalidación (Cache" << id << "): bloque [" << i << "][" << j << "] invalidado.\n";
        }

        // Reemplazar el bloque con el nuevo valor
        data[i][j] = value;
        addr[i][j] = tag;
        valid[i][j] = true;
        dirty[i][j] = true;
        fifo_queue.push({i, j});  // Agregar la nueva posición al final de la cola
    }

    // Métodos para obtener los contadores de eventos
    int get_miss_count() const {
        return miss_count;
    }

    int get_invalidation_count() const {
        return inv_count;
    }

    uint64_t read(uint8_t address){
        uint8_t tag=address;
        uint64_t value;

        // Buscar si ya existe el bloque (para actualizar si hay cache hit)
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (valid[i][j] && addr[i][j] == tag ) { // Cache hit
                    cout << "Cache hit(Cache " << id << "): leyendo dato en [" << i << "][" << j << "].\n";
                    return data[i][j];
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (!valid[i][j]) { // Encontrar un bloque inválido
                    uint64_t load_data=0; //aqui se debe cargar el valor desde la memoria
                    data[i][j] = load_data; //aqui se debe leer el valor de memoria
                    addr[i][j] = tag;
                    valid[i][j] = true;
                    miss_count++;
                    fifo_queue.push({i, j});
                    return data[i][j];
                }
            }
        }

        // Si no hay bloques inválidos (cache llena), reemplaza un bloque (usamos fifo)
        pair<int, int> old_position = fifo_queue.front();  // Obtener el bloque más antiguo
        fifo_queue.pop();  // Eliminar el bloque de la cola FIFO
        int i = old_position.first;
        int j = old_position.second;

        // Si el bloque antiguo está "dirty", hacer write-back a memoria
        if (dirty[i][j]) {
            cout << "Write-back(Cache " << id << "): escribiendo datos del bloque [" << i << "][" << j << "] a memoria.\n";
            // Aquí iría la lógica para escribir a memoria
        }

        // Incrementar el contador de invalidaciones si se está reemplazando un bloque válido
        if (valid[i][j]) {
            inv_count++;
            cout << "Invalidación(Cache " << id << "): bloque [" << i << "][" << j << "] invalidado.\n";
        }

        // Reemplazar el bloque con el nuevo valor
        data[i][j] = 0;
        addr[i][j] = tag;
        valid[i][j] = true;
        dirty[i][j] = false;
        fifo_queue.push({i, j});  // Agregar la nueva posición al final de la cola
    }

    int get_id(){
        return id;
    }
};
