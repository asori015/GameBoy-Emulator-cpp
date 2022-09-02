#pragma once

#include "machine.h"

class Machine;

class GPU {
    public:
     GPU(Machine* machine) {
         this->machine = machine;
     }

     Machine* machine;
     int* greyPallete;
     int* bgPalColor;
     int* obPalColor;
};
