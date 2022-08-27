#include <iostream>
#include "olcConsoleGameEngine.h"
#include "machine.h"

class OneLoneCoder_Example: public olcConsoleGameEngine{
    public:
     bool OnUserCreate();
     bool OnUserUpdate(float );
    private:
};

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

    OneLoneCoder_Example game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();

}

int main(){
    run();
    return 0;
}
