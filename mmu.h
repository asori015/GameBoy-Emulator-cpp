#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include "bootroms.h"

class MMU {
public:
    MMU();
    uint8_t* addrBus(uint16_t);
    void loadBIOS(const uint8_t*, int, uint16_t);
    void loadGameROM(std::string);

    bool BIOSMapped;
private:
    uint8_t* BIOS;         // 0x0000 to 0x0100 or 0x1000
    uint8_t* fixedBank_00; // 0x0000 to 0x3FFF
    uint8_t* fixedBank_NN; // 0x4000 to 0x7FFF
    uint8_t* VRAM;         // 0x8000 to 0x9FFF
    uint8_t* ExtRAM;       // 0xA000 to 0xBFFF
    uint8_t* WRAM_0;       // 0xC000 t0 0xCFFF
    uint8_t* WRAM_N;       // 0xD000 to 0xDFFF
    uint8_t* registers;    // 0xFF00 to 0xFFFF
};
