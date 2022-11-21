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

    addressBus_ = new uint8_t[0xFFFF]{ 0 };
    
    this->cpu = new CPU(this, addressBus_);
    this->gpu = new GPU(this, addressBus_);
}

uint8_t* Machine::getFrame() {
    while (!gpu->getVBLANK()) {
        cpu->step();
        gpu->step();
    }

    uint8_t x = 0;
    return &x;
}