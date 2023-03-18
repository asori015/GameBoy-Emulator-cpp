#include "mmu.h"

MMU::MMU() {
    this->BIOS = new uint8_t[0x0900]{ 0 };
    this->fixedBank_00 = new uint8_t[0x4000]{ 0 };
    this->fixedBank_NN = new uint8_t[255 * 0x4000]{ 0 };
    this->VRAM = new uint8_t[2 * 0x2000]{ 0 };
    this->ExtRAM = new uint8_t[0x2000]{ 0 };
    this->WRAM_0 = new uint8_t[0x1000]{ 0 };
    this->WRAM_N = new uint8_t[7 * 0x1000]{ 0 };
    this->registers = new uint8_t[0x0200]{ 0 };

    this->BIOSMapped = true;

    // loadBIOS() depends on loadGameROM() being completed first
    this->loadGameROM("");
    //this->loadBIOS(BootROMs::BIOS_CGB, 2048, 0);
    this->loadBIOS(BootROMs::BIOS_DMG, 256, 0);
}

// Main function for accessing and manipulating data in RAM
uint8_t* MMU::addrBus(uint16_t address) {
    int index = address / 0x1000;
    switch (index)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        if (BIOSMapped) {
            return &(BIOS[address]);
        }
        else {
            return &(fixedBank_00[address]);
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        return &(fixedBank_NN[address - 0x4000]);
        break;
    case 8:
    case 9:
        return &(VRAM[address - 0x8000]);
        break;
    case 10:
    case 11:
        return &(ExtRAM[address - 0xA000]);
        break;
    case 12:
        return &(WRAM_0[address - 0xC000]);
        break;
    case 13:
        return &(WRAM_N[address - 0xD000]);
        break;
    case 14:
        return &(WRAM_0[address - 0xE000]);
        break;
    case 15:
    default:
        if (address >= 0xFE00) {
            return &(registers[address - 0xFE00]);
        }
        else {
            return &(WRAM_N[address - 0xF000]);
        }
        break;
    }
}

uint8_t MMU::readAddr(uint16_t addr) {
    int index = addr >> 13;
    switch (index)
    {
    case 0: // 0x0000 -> 0x1fff
        int mbctype;
        switch (mbctype)
        {
        case 1:
        case 3:
        case 5:
            break;
        case 2:
            break;
        default:
            break;
        }
        break;
    case 1: // 0x0000 -> 0x3fff
    case 2:
    case 3: // 0x4000 -> 0x7fff
        return x;
        break;
    default:
        break;
    }
}

void MMU::writeAddr(uint16_t addr, uint8_t val) {
    int index = addr >> 13;
    switch (index)
    {
    case 0: // 0x0000 -> 0x1fff
        int mbctype;
        switch (mbctype)
        {
        case 1:
        case 3:
        case 5:
            break;
        case 2:
            break;
        default:
            break;
        }
        break;
    case 1: // 0x0000 -> 0x3fff
    case 2:
    case 3: // 0x4000 -> 0x7fff
        if (BIOSMapped) {
            //return &(BIOS[address]);
        }
        else {
            //return &(fixedBank_00[address]);
        }
        break;
    default:
        break;
    }
}

void MMU::loadBIOS(const uint8_t* ROM, int size, uint16_t address) {
    if (size > 0x100) {
        for (uint16_t i = 0; i < 0x100; i++) {
            BIOS[i] = ROM[i];
        }

        for (uint16_t i = 0x100; i < 0x200; i++) {
            BIOS[i] = fixedBank_00[i];
        }

        for (uint16_t i = 0x100; i < size; i++) {
            BIOS[i + 0x100] = ROM[i];
        }
    }
    else {
        for (uint16_t i = 0; i < size; i++) {
            BIOS[i] = ROM[i];
        }
    }
}

void MMU::loadGameROM(std::string filePath) {
    //filePath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    //filePath = "D:\\Games\\GBA\\Tetris\\Tetris.gb";
    filePath = "D:\\Games\\GBA\\dmg-acid2.gb";
    //filePath = "D:\\Games\\GBA\\cpu_instrs.gb";
    //filePath = "D:\\Games\\GBA\\01-special.gb";
    //filePath = "D:\\Games\\GBA\\02-interrupts.gb";
    //filePath = "D:\\Games\\GBA\\03-op sp,hl.gb";
    //filePath = "D:\\Games\\GBA\\04-op r,imm.gb";
    //filePath = "D:\\Games\\GBA\\05-op rp.gb";
    //filePath = "D:\\Games\\GBA\\06-ld r,r.gb";
    //filePath = "D:\\Games\\GBA\\07-jr,jp,call,ret,rst.gb";
    //filePath = "D:\\Games\\GBA\\08-misc instrs.gb";
    //filePath = "D:\\Games\\GBA\\09-op r,r.gb";
    //filePath = "D:\\Games\\GBA\\10-bit ops.gb";
    //filePath = "D:\\Games\\GBA\\11-op a,(hl).gb";
    std::ifstream gameFile(filePath, std::ios::binary);
    gameFile.read((char*)(fixedBank_00), 0x4000);
    for (int i = 0; i < 0x900; i++) {
        BIOS[i] = fixedBank_00[i];
    }
    gameFile.read((char*)(fixedBank_NN), 0x4000);
    gameFile.close();
}