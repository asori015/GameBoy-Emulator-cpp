#pragma once

#include <map>
#include <functional>
#include "machine.h"

class Machine;

class CPU {
public:
    typedef void (CPU::* functionPointer)(uint8_t );

    CPU(Machine* );
    void step();
    void execute(uint8_t );

private:
    struct Registers {
        uint8_t* REGS = new uint8_t[8]();

        std::function<uint16_t()> AF = [&]()->uint16_t {return (REGS[7] << 8) + REGS[6]; };
        std::function<uint16_t()> BC = [&]()->uint16_t {return (REGS[0] << 8) + REGS[1]; };
        std::function<uint16_t()> DE = [&]()->uint16_t {return (REGS[2] << 8) + REGS[3]; };
        std::function<uint16_t()> HL = [&]()->uint16_t {return (REGS[4] << 8) + REGS[5]; };

        // The actual flag register is REGS[6]
        uint8_t flags = 0;
    };

    void INC(uint8_t );
    void LD(uint8_t );
    void ADD(uint8_t );

    Machine* machine;
    Registers* registers;
    static std::map<uint8_t, functionPointer> OpCodeMethods;
    uint8_t* AddressBus { new uint8_t[0xFFFF]{} };
    uint16_t PC;
    uint16_t SP;
    bool isHalted = false;
};