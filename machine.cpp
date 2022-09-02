#include <iostream>
#include "machine.h"
#include "bootROMs.h"

Machine::Machine(std::string ROMPath, MachineModeEnum mode, std::string savePath) {
    this->timer = new Timer(this);
    this->keypad = new Keypad(this);

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
    //throw Machine::ROMException();
}

std::string Machine::saveExtension(std::string ROMPath) {
    return "";
}