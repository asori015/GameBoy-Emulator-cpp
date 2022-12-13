#include <iostream>
#include <fstream>
#include "machine.h"
#include "test.h"

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <bitset>

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;
const int PIXEL_ZOOM_X = 3;
const int PIXEL_ZOOM_Y = 3;
const uint8_t colorMap[32] = { 0X00, 0X08, 0X10, 0X18, 0X20, 0X29, 0X31, 0X39,
                               0X41, 0X4A, 0X52, 0X5A, 0X62, 0X6A, 0X73, 0X7B,
                               0X83, 0X8B, 0X94, 0X9C, 0XA4, 0XAC, 0XB4, 0XBD,
                               0XC5, 0XCD, 0XD5, 0XDE, 0XE6, 0XEE, 0XF6, 0XFF};

const int BUFFER_SIZE = (SCREEN_HEIGHT * SCREEN_WIDTH * 3) + 12;
uint8_t buffer[BUFFER_SIZE];
Machine* machine;

void initFrame() {
    glClearColor(0.07f, 0.13f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPixelZoom(PIXEL_ZOOM_X, PIXEL_ZOOM_Y);
}

int index = 0;
void renderFrame() {
    // Will get back 144 * 160 16bits per pixel
    uint16_t* frame = machine->getFrame();

    for (int i = 0; i < 144; i++) {
        int position = ((143 - i) * 160 * 3);
        for (int j = 0; j < 160; j++) {
            buffer[position++] = colorMap[(frame[(i * 160) + j] & 0x001F)];
            buffer[position++] = colorMap[(frame[(i * 160) + j] & 0x03E0) >> 5];
            buffer[position++] = colorMap[(frame[(i * 160) + j] & 0x7C00) >> 10];
        }
    }

    //buffer = machine->getFrame();
    // tranlate buffer from hex vals to opengl buffer
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glutSwapBuffers();
}

void run(int argc, char** argv){
    // For testing and debugging purposes
    //Test* test = new Test();

    std::string ROMPath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    machine = new Machine(ROMPath);

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
}

int main(int argc, char** argv){
    run(argc, argv);
    return 0;
}
