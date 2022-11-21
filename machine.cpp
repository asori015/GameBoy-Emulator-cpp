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
    frame_ = new uint16_t[1]{ 0 };
    
    this->cpu = new CPU(this, addressBus_);
    this->gpu = new GPU(this, addressBus_, frame_);
}

uint16_t* Machine::getFrame() {
    uint16_t pc = 0;
    while (!gpu->getVBLANK()) {
        cpu->step();
        gpu->step();

        if (pc != cpu->getPC()) {
            printf("0x%x ", pc);
            pc = cpu->getPC();
        }
        //printf("0x%x ", cpu->getPC());
        //printf("0x%x ", addressBus_[0xFF44]);
    }

    return frame_;
}