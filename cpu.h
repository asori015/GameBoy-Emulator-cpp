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

        enum REG {
            A = 7,
            B = 0,
            C = 1,
            D = 2,
            E = 3,
            F = 6,
            H = 4,
            L = 5
        };

        std::function<uint16_t()> getAF = [&]()->uint16_t {return (REGS[A] << 8) + REGS[F]; };
        std::function<uint16_t()> getBC = [&]()->uint16_t {return (REGS[B] << 8) + REGS[C]; };
        std::function<uint16_t()> getDE = [&]()->uint16_t {return (REGS[D] << 8) + REGS[E]; };
        std::function<uint16_t()> getHL = [&]()->uint16_t {return (REGS[H] << 8) + REGS[L]; };

        //std::function<void()> setAF = [&]()->void {return (REGS[A] << 8) + REGS[F]; };

        // The actual flag register is REGS[6]
        uint8_t flags = 0;
    };

    void Handle_00_Opcodes(uint8_t );
    void Handle_01_Opcodes(uint8_t );
    void Handle_10_Opcodes(uint8_t );
    void Handle_11_Opcodes(uint8_t );

    Machine* machine;
    Registers* registers;
    static std::map<uint8_t, functionPointer> OpCodeMethods;
    uint8_t* AddressBus { new uint8_t[0xFFFF]{} };
    uint16_t PC;
    uint16_t SP;
    bool isHalted = false;
};
