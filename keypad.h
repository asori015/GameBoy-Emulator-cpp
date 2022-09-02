#pragma once

#include "machine.h"

class Machine;

class Keypad {
public:
    Keypad(Machine* machine) {
        this->machine = machine;
    }

    Machine* machine;
};
