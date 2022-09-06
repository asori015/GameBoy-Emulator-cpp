#pragma once

#include <map>
#include "machine.h"

class Machine;

class CPU {
public:

    CPU(Machine* );
    void step();
    void execute(uint8_t );

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

    enum class ISA {
        LD = 3,
        NOP,
        ADD
    };

    void INC();
    void LD(uint8_t, uint8_t);
    void ADD();

    Machine* machine;
    Registers* registers;
    static std::map<uint8_t, void(CPU::*)()> OpCodeMethods;
    uint8_t* AddressBus { new uint8_t[0xFFFF]{} };
    uint16_t PC;
    uint16_t SP;
    bool isHalted = false;
};
