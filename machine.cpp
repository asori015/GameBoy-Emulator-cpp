#include <iostream>
#include <fstream>
#include <vector>
#include "machine.h"

Machine::Machine(std::string ROMPath, MachineModeEnum mode, std::string savePath) {
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
    
    this->gpu = new GPU(this, usingColor, monochromeCompatibility);
    this->cpu = new CPU(this);
}
