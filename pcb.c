#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

/*
Passes 2 parameters (start , end)
This method creates a PCB with fields set as this:
PC = start
start = start
end = end
*/
PCB* makePCB(int start, int end){
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->PC = start;
    pcb->start = start;
    pcb->end = end;

    for(int i = 0; i < 10; i++){
        pcb->pageTable[i] = -1;
    }

    return pcb;
}




