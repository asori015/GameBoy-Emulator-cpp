#pragma once

#include "machine.h"

class Machine;
class CPU;

class GPU {
public:
    GPU(Machine*, uint8_t*);
    void step();
    bool getVBLANK();
private:
    uint8_t* addressBus_;
    bool VBLANK;
};
