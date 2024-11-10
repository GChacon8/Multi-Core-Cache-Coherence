#include <iostream>  // Librería para entrada/salida
#include <bitset>  // Librería para bitset
#include <cstdint>
#include "PE.cpp"
#include "RAM.cpp"
using namespace std;

int main() {
    PE core1=PE(1);
    PE core2=PE(2);
    PE core3=PE(3);
    PE core4=PE(4);

    //Ram ram();

    core1.set_reg(0,1);
    core1.set_reg(1,2);
    core1.set_reg(2,3);

    core1.WRITE(1,0);
    core1.READ(3,0);
    uint64_t x=core1.get_reg(3);

    core1.WRITE(2,0);
    core1.READ(3,0);
    uint64_t y=core1.get_reg(3);

    core1.WRITE(1,1);
    core1.READ(3,1);
    uint64_t z=core1.get_reg(3);

    cout << "x = [" << x << "]\n";
    cout << "y = [" << y << "]\n";
    cout << "z = [" << z << "]\n";
}
