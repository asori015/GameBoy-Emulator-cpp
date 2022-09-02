#pragma once

#include "machine.h"

class Machine;

class MMU {
public:
    MMU(Machine* machine) {
        this->machine = machine;
    }

    Machine* machine;
};
