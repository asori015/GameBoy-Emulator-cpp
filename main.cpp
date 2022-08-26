#include <iostream>
#include "machine.h"

class Screen{
    public:
     Screen(Machine* x);
    private:
     Machine* x;
};

Screen::Screen(Machine* x){
    this->x = x;
}

void run(){
    std::string ROMPath = "D:\\Games\\GBA\\pokemon\\vanilla\\Pokemon red.gb";
    Machine* machine = new Machine("Hello GameBoy Emulator!");
    Screen* screen = new Screen(machine);
    //screen.keypad = machine.getKeypad();
}

int main(){
    run();
    return 0;
}
