#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"
#include "memorymanager.h"

#define DEFAULT_QUANTA 2


struct CPU { 
    int IP; 
    char IR[1000]; 
    int quanta;
    int offset;
} CPU;
int page_fault(PCB *pcb);

/*
This method passes a quanta.
It will then executes quanta number of lines in RAM as from the address
pointed by the CPU IP.
Returns an errorCode.
*/
int run(PCB *pcb, int quanta){
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    if (quanta > DEFAULT_QUANTA ){
        quanta = DEFAULT_QUANTA;
    }

//    CPU.offset = 0;


    for (int i = 0; i < quanta; i++)
    {
        CPU.offset = pcb->PC_offset; // TODO
        if(CPU.offset >= 4 || pcb->PC_offset >= 4){
//            break;
            CPU.offset = 0;
            return page_fault(pcb);
        }

//        CPU.IP = pcb->pageTable[] + pcb->PC;
//        CPU.IP = pcb->PC;
        int addr = CPU.IP + CPU.offset;
//        printf("%s", ram[addr]);
        strcpy(CPU.IR, ram[addr]);


//        printf("IP is %d, CPU Offset is %d\n", CPU.IP, CPU.offset);
//        printf("%s\n", ram[addr]);

//        printf("Executed %s", CPU.IR);
        int errorCode = parse(CPU.IR);
        // Do error checking and if error, return error
        if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                ram[addr][strlen(ram[addr])-2]='\0';
                displayCode(errorCode,ram[addr]);
            }
            return errorCode;
        }
//        CPU.IP ++;
        CPU.offset += 1;
        pcb->PC_offset = CPU.offset;
    }


    return 0;
}

int page_fault(PCB *pcb){
    int flag = 0;
    int frame = -1;
    int victim = -1;
    char line[1000];
//    CPU.offset = 0;


    // 1. Determine the next page
    pcb->PC_page ++;

    // 2. If beyong max then terminate.
    if(pcb->PC_page >= pcb->pages_max){
        pcb->PC_page --;
        return -1;
    }
    // Else we check if the frame for the page exists
    // TODO: QUESTIONABLE CODE PROBABLY BUG HERE
    else{
//        frame = CPU.IP; // The frame# is the current IP. IP = 0 ... 36
//        for(int i = 0; i < pcb->pages_max; i++){
//            if(pcb->pageTable[i] == frame){
//                flag = 1;
//                break;
//            }
//        }
        flag = 1;
        if(pcb->pageTable[pcb->PC_page] == -1){
            // Get the frame number from the page table
            frame = pcb->pageTable[pcb->PC_page];
            flag = 0;
        }

    }

    // Frame is valid
    if(flag == 1){
        pcb->PC = frame;
        pcb->PC_offset = 0;
    }
    // Frame is not valid, we need to find a page
    else{
        // (a) find page in backing store
        FILE *page = fopen(pcb->pid, "r");

        // (b) find sapce in ram
        frame = findFrame();
        if(frame == -1){
            victim = findVictim(pcb);
        }

        // (c) update the page tables
        updatePageTable(pcb, pcb->PC_page, frame, victim);

        // (d) Update RAM frame

        // Find line by page number

        for(int i = 0; i < pcb->PC_page*4; i++){
            fgets(line, 1000, page);
        }

        int offset = 0;
        // Load 4 pages into ram
        while(fgets(line, 1000, page) && offset < 4){
            ram[pcb->pageTable[pcb->PC_page] * 4 + offset] = strdup(line);
//            printf("%sSaved to ram[%d]\n", line, pcb->pageTable[pcb->PC_page] * 4 + offset);
//            pcb->PC_offset += 1;
            offset += 1;
        }

        // (e) TODO
        pcb->PC = pcb->pageTable[pcb->PC_page];
        pcb->PC_offset = 0;


    }

    return 0;



}