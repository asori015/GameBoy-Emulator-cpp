#pragma once

#include "machine.h"

class Machine;
class CPU;

class GPU {
public:
    GPU(Machine*, CPU* );
    void step();
private:
    ;
};
