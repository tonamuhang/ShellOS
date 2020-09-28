//
// Created by Muhang, Li on 2020-04-08.
//

#include "memorymanager.h"

int file_count = 0;

// Return the total number of pages needed
int countTotalPages(FILE *f){
    int line = 0;
    char code[1000];
    int page = 0;

    while(fgets(code, 1000, f)){
        line += 1;
    }

    // Return to head of file
    fseek(f, 0, SEEK_SET);

    if(line % PAGE_SIZE == 0){
        page = line / PAGE_SIZE;
    }
    else{
        page = line / PAGE_SIZE + 1;
    }

    return page;
}

/*
 *  Use the FIFO technique to search ram[] for a frame (not equal to NULL). If one
 *  exists then return its index number(Frame number), otherwise return -1.
 */
int findFrame(){
    int i = 0;
    int frame_num = 0;
    for(i = 0; i < RAM_SIZE; i+=FRAME_SIZE){
        if(!ram[i]){
            return frame_num;
        }
        frame_num += 1;
    }

    return -1;

}


/*
 * This function is only invoke when findFrame() returns a -1. Use a random
 * number generator to pick a random frame number. If the frame number does not
 * belong to the pages of the active PCB (ie. it is not in its page table) then return that
 * frame number as the victim, otherwise, starting from the randomly selected frame,
 * iteratively increment the frame number (modulo-wise) until you come to a frame
 * number not belong to the PCB’s pages, and return that number.
 */
int findVictim(PCB *p){
    time_t t;
    srand((unsigned) time(&t));
    int frame_num = rand() % 10;
    int flag = 0;
//    if(frame_num < p->start || frame_num > p->end){
//        return frame_num;
//    }
//
//    else{
//        while (frame_num <= p->end){
//            frame_num += FRAME_SIZE;
//        }
//        return frame_num;
//    }

    // Search if the frame num is in the pcb's pages
    for(int i = 0; i < p->pages_max; i++){
        if(p->pageTable[i] == frame_num){
            flag = 1;
            break;
        }
    }

    // If frame number doesnt belong to the pcb
    if(flag == 0){
        return frame_num;
    }
    else{
        while(flag == 1){
            flag = 0;

            // Search if the frame num is in the pcb's pages
            for(int i = 0; i < p->pages_max; i++){
                if(p->pageTable[i] == frame_num){
                    flag = 1;
                    break;
                }
            }

            frame_num = (frame_num+1)%10;

        }
        return frame_num;
    }

}


/*
 * FILE *f points to the beginning of the file in the backing store. The variable
 * pageNumber is the desired page from the backing store. The function loads the 4
 * lines of code from the page into the frame in ram[].
 */
void loadPage(int pageNumber, FILE *f, int frameNumber){
    if(!f){
        return;
    }

    fseek(f, 0, SEEK_SET);  // Points to the beginning of the file

    char line[1000];
    int offset = 0;

    for(int i = 0; i < pageNumber * 4; i++){
        fgets(line, 1000, f);
    }


    // Load 4 lines of code
    int count = 0;
    while(fgets(line, 1000, f) && count < 4){
        ram[frameNumber * 4 + offset] = strdup(line);
//        printf("%sSaved to ram[%d].\n", ram[frameNumber * 4 + offset], frameNumber * 4 + offset);
        offset += 1;
        count += 1;
    }

}


int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
    // if a victim is found
    if(frameNumber == -1) {
        p->pageTable[pageNumber] = victimFrame;

        // Find the pcb of the victim frame
        for(int i = 0; i < 3; i++){
            if(pcbtable[i] != NULL && pcbtable[i] != p){
                PCB *victim_pcb = pcbtable[i];
                // Update the page table
                for(int j = 0; j < 10; j++){
                    if(victim_pcb->pageTable[j] == frameNumber){
                        victim_pcb->pageTable[j] = -1;
                    }
                }
            }
        }
    }
    else{
        p->pageTable[pageNumber] = frameNumber;
    }



    return 0;
}


int launcher(FILE *p){
    // 1. Copy the entire file into the backing store.
    char filename[1000];
    char line[1000];
    int page_count = 0;
    int totalpages = countTotalPages(p);
    // Copy entire file into the backing store
//    while (page_count < totalpages){
        switch(file_count){
            case 0:
                sprintf(filename, "BackingStore/script1.txt");
                break;
            case 1:
                sprintf(filename, "BackingStore/script2.txt");
                break;
            case 2:
                sprintf(filename, "BackingStore/script3.txt");
                break;
            default:
                sprintf(filename, "BackingStore/script1.txt");
                file_count = 0;
        }

//        sprintf(filename, "%s_%d.txt", filename, page_count);


        // Write to file
        int line_count = 0;
        FILE *new_file = fopen(filename, "w");

        while(fgets(line, 1000, p)){ //&& line_count < PAGE_SIZE
            fprintf(new_file, "%s", line);

            line_count += 1;
        }
        fclose(new_file);


        // Record the filename
//        filenames[file_count][page_count] = strdup(filename);

//        printf("%s\n", filenames[file_count][0]);
//    }


    // 2. Close the file ptr to the original
    fclose(p);


    // 3. Open the file in the backing store
    FILE *page = fopen(filename, "r");

    // 4. Load pages into RAM
    PCB* pcb = makePCB(0, 0);
    pcbtable[file_count] = pcb;

    int frame_num = findFrame();
    int victimFrame = 0;

    // If no free frame find a victim
    if(frame_num == -1){
        victimFrame = findVictim(pcb);
    }


    // TODO
    pcb->pages_max = totalpages;
    pcb->PC_page = 0;
    pcb->PC = frame_num; // *4
    pcb->PC_offset = 0;
    pcb->pid = strdup(filename);

    addToReady(pcb);
    loadPage(pcb->PC_page, page, frame_num);
    updatePageTable(pcb, pcb->PC_page, frame_num, victimFrame);

    if(totalpages>1){
        frame_num = findFrame();
        if(frame_num == -1){
            victimFrame = findVictim(pcb);
        }

        pcb->PC_page = 1;
//        pcb->PC = frame_num * 4;
        pcb->PC_offset = 0;

        addToReady(pcb);
        loadPage(pcb->PC_page, page, frame_num);
        updatePageTable(pcb, pcb->PC_page, frame_num, victimFrame);
        pcb->PC_page = 0;
    }
    fclose(page);


    file_count += 1;
    return 0;


}






