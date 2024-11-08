#include <iostream>  // Librería para entrada/salida
#include <bitset>  // Librería para bitset
#include <cstdint>
#include "PE.cpp"
#include "RAM.cpp"
using namespace std;

int main() {
    PE core1(1);
    PE core2(2);
    PE core3(3);
    PE core4(4);

    //Ram ram();

    core1.set_reg(1,1);
    core1.set_reg(2,2);
    core1.set_reg(3,3);
    core1.set_reg(4,4);
    core1.WRITE(1,0);
    core1.WRITE(2,0);
    core1.WRITE(1,1);
}
