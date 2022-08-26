#include <iostream>
#include "machine.h"

void run(){
    std::string ROMPath = "D:\\Games\\GBA\\pokemon\\vanilla\\Pokemon red.gb";
    Machine* machine = new Machine("Hello GameBoy Emulator!");
    machine->hello();
}

int main(){
    run();
    return 0;
}
