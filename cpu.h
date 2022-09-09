#pragma once

#include <map>
#include <functional>
#include "machine.h"

class Machine;

class CPU {
public:
    typedef void (CPU::* functionPointer)(uint8_t);

    CPU(Machine*);
    void step();
    void execute(uint8_t);

private:
    enum Register
    {
        A = 7,
        B = 0,
        C = 1,
        D = 2,
        E = 3,
        F = 6,
        H = 4,
        L = 5
    };

    void loadBIOS(uint16_t);
    void run();
    
    // OpCode implementation functions
    void nop(uint8_t);
    void LD_R_to_R(uint8_t);
    void LD_8_Bit(uint8_t);
    void LD_16_Bit(uint8_t);
    void ADD(uint8_t);
    void ADC(uint8_t);
    void SUB(uint8_t);
    void SBC(uint8_t);
    void AND(uint8_t);
    void XOR(uint8_t);
    void OR(uint8_t);
    void CP(uint8_t);
    void debug();

    // Register helper functions
    uint16_t getAF();
    uint16_t getBC();
    uint16_t getDE();
    uint16_t getHL();

    void setAF(uint8_t, uint8_t);
    void setBC(uint8_t, uint8_t);
    void setDE(uint8_t, uint8_t);
    void setHL(uint8_t, uint8_t);

    void setAF(uint16_t);
    void setBC(uint16_t);
    void setDE(uint16_t);
    void setHL(uint16_t);

    void setC(bool );
    void setH(bool );
    void setN(bool );
    void setZ(bool );

    Machine* machine;
    uint8_t* registers = new uint8_t[8]{ 0 };
    uint8_t* addressBus = new uint8_t[0xFFFF]{ 0 };
    static std::map<uint8_t, functionPointer> InstructionMethods1;
    static std::map<uint8_t, functionPointer> InstructionMethods2;
    uint16_t PC;
    uint16_t SP;
    bool isHalted = false;
};
