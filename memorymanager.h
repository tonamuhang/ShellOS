//
// Created by Muhang, Li on 2020-04-08.
//

#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "ram.h"
#include "string.h"
#include "kernel.h"

int launcher(FILE *);
//char* filenames[3][10];

PCB *pcbtable[3];
int findFrame();
int findVictim(PCB*);
int updatePageTable(PCB *, int, int, int);
int launcher(FILE*);