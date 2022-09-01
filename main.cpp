#include <iostream>
// #include "olcConsoleGameEngine.h"
#include "machine.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <bitset>

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;
const int PIXEL_ZOOM_X = 4;
const int PIXEL_ZOOM_Y = 4;

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


char buffer[160 * 144 * 3 + 12];
void initFrame() {
    glClearColor(0.07f, 0.13f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 160 * 3; j++) {
            int index = (i * 160 * 3) + j;
            buffer[index] = (index % 2) * 128;
        }
    }

    glPixelZoom(PIXEL_ZOOM_X, PIXEL_ZOOM_Y);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glutSwapBuffers();
}

int state = 0;
void renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    char* cbuffer = buffer;

    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, cbuffer + state / 500);
    glutSwapBuffers();
    glutPostRedisplay();
    state = (state + 1) % 1999;
    std::cout << state / 500 << std::endl;
}

void run(int argc, char** argv){
    std::string ROMPath = "D:\\Games\\GBA\\pokemon\\vanilla\\Pokemon red.gb";
    Machine* machine = new Machine("Hello GameBoy Emulator!");
    Screen* screen = new Screen(machine);
    //screen.keypad = machine.getKeypad();

    // Initialize OpenGL/FreeGLUT window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutInitWindowSize(SCREEN_WIDTH * PIXEL_ZOOM_X, SCREEN_HEIGHT * PIXEL_ZOOM_Y);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("GB Emulator");

    // Register OpenGL Callbacks
    glutDisplayFunc(initFrame); // Initial call
    glutIdleFunc(renderFrame); // All other calls

    glutMainLoop();

    // OneLoneCoder_Example game;
    // game.ConstructConsole(160, 100, 8, 8);
    // game.Start();

}

int main(int argc, char** argv){
    run(argc, argv);
    return 0;
}
