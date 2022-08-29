#include <iostream>
// #include "olcConsoleGameEngine.h"
#include "machine.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <bitset>

// class OneLoneCoder_Example: public olcConsoleGameEngine{
//     public:
//      bool OnUserCreate();
//      bool OnUserUpdate(float );
//     private:
// };

class Screen{
    public:
     Screen(Machine* x);
    private:
     Machine* x;
};

Screen::Screen(Machine* x){
    this->x = x;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    char buffer[160 * 144 * 3 + 8];

    for(int i = 0; i < 144; i++){
        for(int j = 0; j < 160 * 3; j++){
            int index = (i * 160  * 3) + j;
            buffer[index] = (index % 2) * 128;
        }
    }

    glPixelZoom(4,4);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glFlush();
}

void run(int argc, char** argv){
    std::string ROMPath = "D:\\Games\\GBA\\pokemon\\vanilla\\Pokemon red.gb";
    Machine* machine = new Machine("Hello GameBoy Emulator!");
    Screen* screen = new Screen(machine);
    //screen.keypad = machine.getKeypad();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowPosition(150, 150);
    glutInitWindowSize(160 * 4, 144 * 4);
    glutCreateWindow("A Simple Triangle");

    glutDisplayFunc(display);

    glutMainLoop();

    // OneLoneCoder_Example game;
    // game.ConstructConsole(160, 100, 8, 8);
    // game.Start();

}

int main(int argc, char** argv){
    run(argc, argv);
    return 0;
}
