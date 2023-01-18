#pragma once

#include <cstdint>
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "timer.h"
#include "keypad.h"
#include "soundboard.h"

class CPU;
class MMU;
class GPU;
class Timer;
class Keypad;

class Machine {
public:
    Machine(std::string);
    uint16_t* getFrame();
private:
    CPU* cpu;
    GPU* gpu;
    MMU* mmu;

    uint8_t* addressBus_;
    uint16_t* frame_;
    bool inVBLANK_;
};