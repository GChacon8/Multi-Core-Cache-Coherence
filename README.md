# MESI Cache Coherence Protocol Simulator
## Description
The objective of this project its to simulate a MESI protocol in a multicore system.
Another important aspect of this project is to visualize the phenomenom called 'false sharing'.
This behavior happens because each cache has write-back and write-allocate policies.

## Components
* Processing Element
* Cache
* RAM (Shared Memory)
* Bus Interconnect

## How it works?
The simulator uses threads to simulate each processing element simultaneously and the bus interconnect process every write or read request using a FIFO policy.
Also, each processing element has its own cache for data and a private ROM where all the instructions are stored, readed and excecuted.
Finaly, the Bus Interconnect controls the state of each cache and updates accordingly the protocol. Also it writes or reads in the shared memory if necessary.

## Getting Started
### Dependencies for Windows
* Windows 10
* QTCreator 14.0.2 (Community)
* Qt 6.8.0 (MinGW 13.1.0 64-bit)

### Dependencies for Ubuntu
* Ubuntu 24.04
* QMake 3.1
* Qt 5.15.13

### How to run the program
### Windows
First, you need to download the repository. You can download the `.zip` file and extract the content wherever you want.
After that, open QTCreator and select the option 'Open Project' and browse the directory of the project.
Once you find it, select the `MultiCoreCacheCoherence.pro` file.
And finally, you just need to run or press `ctrl+R`

### Ubuntu
After downloading the repository, open the directory in a terminal an execute the following command:
```
qmake && make && ./MultiCoreCacheCoherence
```
This will build and run the application

### Authors
[Gabriel Chacon Alfaro](https://github.com/GChacon8)

[Daniel Castro Elizondo](https://github.com/Dcastroe10)

[Jose Eduardo Cruz Vargas](https://github.com/eduardocv30)

[Marco Rivera Serrano](https://github.com/SlimeVRS)
