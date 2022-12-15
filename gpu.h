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

    enum State{Mode0, Mode1, Mode2, Mode3};

    uint8_t* addressBus_;
    uint16_t* frame_;
    bool VBLANK;
    //const uint16_t VRAM_1 = 0x8000;
    //const uint16_t VRAM_2 = 0x8800;
    //const uint16_t VRAM_3 = 0x97FF;

    const uint16_t VRAM = 0x8000; // 
    const uint16_t TILE_MAP1 = 0x9800;
    const uint16_t TILE_MAP2 = 0x9C00;
    const uint16_t OAM = 0xFE00; // Object Attribute Memory

    // LCD Registers
    const uint8_t* const LCDC; // LCD Control Register
    const uint8_t* const STAT; // LCD Status Flag
    const uint8_t* const SCY; // LCD Scroll Y Register
    const uint8_t* const SCX; // LCD Scroll X Register
    uint8_t* const LY; // LCDC Y-coordinate
    const uint16_t LYC = 0xFF45; // LY Compare Register
    const uint16_t DMA = 0xFF46; // DMA Transfer and Starting Address
    const uint16_t BGP = 0xFF47; // BG Palette Data
    const uint16_t OBP0 = 0xFF48; // OBJ Palette Data 0
    const uint16_t OBP1 = 0xFF49; // OBJ Palette Data 1
    const uint16_t WY = 0xFF4A; // Window X-Coordinate
    const uint16_t WX = 0xFF4B; // Window Y-Coordinate

    int state_;
    int clock_;
};
