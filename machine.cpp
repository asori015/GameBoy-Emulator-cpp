#include <iostream>
#include <fstream>
#include <vector>
#include "machine.h"
#include "bootROMs.h"

const int Machine::RAM_SIZES[] = { 0, 1 << 11, 1 << 13, 1 << 15, 1 << 17 };

Machine::Machine(std::string ROMPath, MachineModeEnum mode, std::string savePath) {
    if (savePath == "") {
        savePath = saveExtension(ROMPath);
    }

    this->saveFilePath = savePath;
    this->timer = new Timer(this);
    this->keypad = new Keypad(this);
    this->soundBoard = new SoundBoard();

    switch (mode){
    case Machine::MachineModeEnum::GAMEBOY:
        this->mode = { 1, false, BootROMs::BIOS_DMG };
        break;
    case Machine::MachineModeEnum::GAMEBOY_POCKET:
        this->mode = { 1, false, BootROMs::BIOS_DMG };
        break;
    case Machine::MachineModeEnum::GAMEBOY_COLOR:
        this->mode = { 0x11, true, BootROMs::BIOS_CGB };
        break;
    default:
        //throw Machine::ROMException("No valid machine mode specified");
        break;
    }

    try {
        // Get the header information of the ROM and use it to determine MBC type, RAM/ROM size
        std::ifstream gameROMInput(ROMPath, std::ios::binary);
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(gameROMInput), {});
        
        if (buffer.size() < 0x150) {
            //throw Machine::ROMException();
        }

        int cartridgeType = buffer[0x147] & 0xff;
        int colorMode = buffer[0x143] & 0xff;
        this->usingColor = (this->mode.isCgb && (colorMode & 0x06) == 0);
        this->monochromeCompatibility = usingColor && (colorMode & 0x80) == 0;
        int mbc = 0;
        int romBanks;
        int ramSize = 0;

        if (buffer[0x149] != 0) {
            ramSize = RAM_SIZES[buffer[0x149]];
        }

        // Debug GameROM values to console screen
        std::cout << "Cartridge type = " << (int)cartridgeType
                  << ", ramkey = " << (int)buffer[0x149] 
                  << ", Color? " << usingColor
                  << " (Compatibility? " << monochromeCompatibility << ")" << std::endl;

        switch (cartridgeType) {
        case 0:
            ramSize = 0; break;
        case 1:
            ramSize = 0;
        case 2:
        case 3:
            mbc = 1; break;
        case 5:
        case 6:
            mbc = 2; ramSize = 512; break;
        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            mbc = 3; break;
        case 0x19:
        case 0x1C:
            ramSize = 0;
        case 0x1A:
        case 0x1B:
        case 0x1D:
        case 0x1E:
            mbc = 5; break;
        }

        romBanks = buffer[0x148];
        if (romBanks <= 8)
            romBanks = 1 << (1 + romBanks);
        else switch (romBanks) {
        case 0x52:
            romBanks = 72; break;
        case 0x53:
            romBanks = 80; break;
        case 0x54:
            romBanks = 96; break;
        }

        // Create the memory component
        this->gpu = new GPU(this);
        this->mmu = new MMU(this);
        // Load this ROM file into it
        //mmu.loadRom(header, 0, 0x150);
        //mmu.loadRom(fis, 0x150, (romBanks << 14) - 0x150);
        //try {
        //    loadExternal();
        //}
        //catch (RomException re) {
        //    System.err.println("Could not load save file: ");
        //    re.printStackTrace();
        //}

        //std::fstream readGameROM;
        //readGameROM.open(ROMPath, std::ios::binary);
        //int read = readGameROM.read(header, 0x150);
        //Read file
        //int read = 
    }
    catch (...){
        ;
    }

    
    
    this->cpu = new CPU();
}

std::string Machine::saveExtension(std::string ROMPath) {
    int dotIndex = ROMPath.rfind(".");
    if (dotIndex == -1) {
        return ROMPath + ".ram";
    }
    else {
        return ROMPath.substr(0, dotIndex) + ".ram";
    }
}