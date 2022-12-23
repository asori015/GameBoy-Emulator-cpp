#pragma once

#include "machine.h"

class Machine;
class CPU;

class GPU {
public:
    GPU(Machine*, uint8_t*, uint16_t*);
    void step();
private:
    void renderLine();
    void renderBackgroundLine();
    void renderWindowLine();
    void renderObjectLine();
    void clearLine();

    enum State{Mode0, Mode1, Mode2, Mode3};

    uint8_t* addressBus_;
    uint16_t* frame_;

    const uint8_t* const VRAM_1;
    const uint8_t* const VRAM_2;
    const uint8_t* const TILE_MAP1;
    const uint8_t* const TILE_MAP2;
    const uint8_t* const OAM; // Object Attribute Memory
    uint8_t* const IF; // Interupt Flag

    // LCD Registers
    const uint8_t* const LCDC; // LCD Control Register
    uint8_t* const STAT; // LCD Status Flag
    const uint8_t* const SCY; // LCD Scroll Y Register
    const uint8_t* const SCX; // LCD Scroll X Register
    uint8_t* const LY; // LCDC Y-coordinate
    const uint8_t* const LYC; // LY Compare Register
    const uint8_t* const DMA; // DMA Transfer and Starting Address
    const uint8_t* const BGP; // BG Palette Data
    const uint8_t* const OBP0; // OBJ Palette Data 0
    const uint8_t* const OBP1; // OBJ Palette Data 1
    const uint8_t* const WY; // Window X-Coordinate
    const uint8_t* const WX; // Window Y-Coordinate
    uint8_t windowLineCounter_;
    const uint16_t colorValues[4] = { 0xFFFF, 0x56B5, 0x294A, 0x0000 };

    int state_;
    int clock_;
    bool statInterupt_;
};
