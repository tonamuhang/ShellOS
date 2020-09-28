#define DEFAULT_QUANTA 2

#include "pcb.h"

struct CPU {
    int IP; 
    char IR[1000]; 
    int quanta;
    int offset;
} CPU;

int run(PCB*, int);