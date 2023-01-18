#include <iostream>
#include <fstream>
#include <vector>
#include "machine.h"

Machine::Machine(std::string ROMPath) {
    /*if (savePath == "") {
        savePath = saveExtension(ROMPath);
    }*/

    //this->saveFilePath = savePath;
    //this->timer = new Timer(this);
    //this->keypad = new Keypad(this);
    //this->soundBoard = new SoundBoard();

    //switch (mode){
    //case Machine::MachineModeEnum::GAMEBOY:
    //    //this->mode = { 1, false, BootROMs::BIOS_DMG };
    //    break;
    //case Machine::MachineModeEnum::GAMEBOY_POCKET:
    //    //this->mode = { 1, false, BootROMs::BIOS_DMG };
    //    break;
    //case Machine::MachineModeEnum::GAMEBOY_COLOR:
    //    //this->mode = { 0x11, true, BootROMs::BIOS_CGB };
    //    break;
    //default:
    //    //throw Machine::ROMException("No valid machine mode specified");
    //    break;
    //}

    this->addressBus_ = new uint8_t[0xFFFF]{ 0 };
    this->frame_ = new uint16_t[160 * 144]{ 0 };
    
    this->mmu = new MMU();
    this->cpu = new CPU(mmu);
    this->gpu = new GPU(mmu, frame_);
    this->inVBLANK_ = false;
}

uint16_t* Machine::getFrame() {
    while (*mmu->addrBus(0xFF44) >= 0x90 && inVBLANK_) {
        cpu->step();
        gpu->step();
    }

    inVBLANK_ = false;

    //addressBus_[0xFF40] = 0x91;
    while (*mmu->addrBus(0xFF44) < 0x90 && !inVBLANK_) {
        cpu->step();
        gpu->step();
    }

    inVBLANK_ = true;

    return frame_;
}