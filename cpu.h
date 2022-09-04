#pragma once

#include "machine.h"

class Machine;

class CPU {
public:

    CPU(Machine*);
    void execute();

private:
    struct Registers {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t h;
        uint8_t l;
        uint16_t af;
        uint16_t bc;
        uint16_t de;
        uint16_t hl;
        uint8_t flags = 0;
    };

    Machine* machine;
    Registers* registers;
};
