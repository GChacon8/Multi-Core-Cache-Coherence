#include <iostream>
#include <bitset>  // Librería para bitset
#include <stdbool.h>  // Incluir la biblioteca para usar booleanos
using namespace std;

struct inst
{
    int inst;
    int reg_num;
    int addr;
    bool cond;
};


class Rom {
private:

    string example;

public:
    
    // Métodos para obtener los atributos
    string get_example() {
        return example;
    }

    void set_example(string value) {
        example = value;
    }

};
