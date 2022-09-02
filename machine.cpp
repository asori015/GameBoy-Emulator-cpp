#include <iostream>
#include "machine.h"

Machine::Machine(std::string msg, MachineModeEnum mode){
    //this->msg = msg;
    switch (mode){
    case Machine::MachineModeEnum::GAMEBOY:
        this->mode = { 1, false, "a" };
        break;
    case Machine::MachineModeEnum::GAMEBOY_POCKET:
        break;
    case Machine::MachineModeEnum::GAMEBOY_COLOR:
        break;
    default:
        //throw Machine::ROMException("No valid machine mode specified");
        break;
    }
    //throw Machine::ROMException();
}

void Machine::hello(){
    //std::cout << this->msg << std::endl;
}