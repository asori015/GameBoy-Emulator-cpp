#pragma once

#include "machine.h"

class Machine;

class Timer {
public:
    Timer(Machine* machine) {
        this->machine = machine;
    }

    Machine* machine;
};
