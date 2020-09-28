//
// Created by Muhang, Li on 2020-04-08.
//

#include "osboot.h"

int main(){


    int error = 0;
    boot();
    error = kernel();
    return error;
}