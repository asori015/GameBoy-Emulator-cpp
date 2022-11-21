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
    uint8_t* getFrame();
private:
    CPU* cpu;
    GPU* gpu;

    uint8_t* addressBus_;
};