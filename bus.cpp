#include <iostream>
using namespace std;

class Bus {
private:

    int inv_count;
    int read_count;
    int write_count;
    int data_count[4];

public:
    
    // Métodos para obtener los atributos
    int get_inv_count() {
        return inv_count;
    }

    int get_read_count() {
        return read_count;
    }

    int get_write_count() {
        return write_count;
    }

    int get_data_count(int core_num) {
        return data_count[core_num];
    }
};