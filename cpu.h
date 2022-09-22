#pragma once

#include <map>
#include <functional>
#include "machine.h"

class Machine;

class CPU {
public:
    typedef void (CPU::* FunctionPointer)(uint8_t, uint8_t, uint8_t);

    CPU(Machine*);
    void loadBIOS(const uint8_t*, int, uint16_t);
    void step();
    void execute(uint8_t);
    void printRegs();

    // Getter Functions for Register
    uint16_t getAF();
    uint16_t getBC();
    uint16_t getDE();
    uint16_t getHL();
    uint16_t getSP();

    bool getC();
    bool getH();
    bool getN();
    bool getZ();

    uint8_t* addressBus_ = new uint8_t[0xFFFF]{ 0 };
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

    void run(); // Temporary

    // OpCode implementation functions
    void nop(uint8_t, uint8_t, uint8_t);
    void LD_R_to_R(uint8_t, uint8_t, uint8_t);
    void LD_8_Bit(uint8_t, uint8_t, uint8_t);
    void LD_16_Bit(uint8_t, uint8_t, uint8_t);
    void JP(uint8_t, uint8_t, uint8_t);
    void JR(uint8_t, uint8_t, uint8_t);
    void ADD(uint8_t, uint8_t, uint8_t);
    void SUB(uint8_t, uint8_t, uint8_t);
    void AND(uint8_t, uint8_t, uint8_t);
    void XOR(uint8_t, uint8_t, uint8_t);
    void OR(uint8_t, uint8_t, uint8_t);
    void CP(uint8_t, uint8_t, uint8_t);
    void INC(uint8_t, uint8_t, uint8_t);
    void DEC(uint8_t, uint8_t, uint8_t);
    void ADD_16_BIT(uint8_t, uint8_t, uint8_t);
    void INC_16_BIT(uint8_t, uint8_t, uint8_t);
    void DEC_16_BIT(uint8_t, uint8_t, uint8_t);
    void CBPrefix(uint8_t, uint8_t, uint8_t);
    void RLC(uint8_t, uint8_t, uint8_t);
    void RRC(uint8_t, uint8_t, uint8_t);
    void RL(uint8_t, uint8_t, uint8_t);
    void RR(uint8_t, uint8_t, uint8_t);
    void SLA(uint8_t, uint8_t, uint8_t);
    void SRA(uint8_t, uint8_t, uint8_t);
    void SWAP(uint8_t, uint8_t, uint8_t);
    void SRL(uint8_t, uint8_t, uint8_t);
    void BIT(uint8_t, uint8_t, uint8_t);
    void RES(uint8_t, uint8_t, uint8_t);
    void SET(uint8_t, uint8_t, uint8_t);
    void CALL(uint8_t, uint8_t, uint8_t);
    void RET(uint8_t, uint8_t, uint8_t);

    // Register helper functions

    void setAF(uint8_t, uint8_t);
    void setBC(uint8_t, uint8_t);
    void setDE(uint8_t, uint8_t);
    void setHL(uint8_t, uint8_t);

    void setAF(uint16_t);
    void setBC(uint16_t);
    void setDE(uint16_t);
    void setHL(uint16_t);

    void setC(bool);
    void setH(bool);
    void setN(bool);
    void setZ(bool);

    Machine* machine_;
    uint8_t* registers_ = new uint8_t[8]{ 0 };
    static std::map<uint8_t, FunctionPointer> instructionMethods1_;
    static std::map<uint8_t, FunctionPointer> instructionMethods2_;
    uint16_t PC_;
    uint16_t SP_;
    bool isHalted_ = false;
    // Only for debug print statements
    bool debug_ = false;
    const char regNames_[8] = {'B', 'C', 'D', 'E', 'H', 'L', 'F', 'A'};
};
