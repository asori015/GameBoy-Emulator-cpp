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

const int BUFFER_SIZE = (160 * 144 * 3) + 12;
uint8_t buffer[BUFFER_SIZE];
Machine* machine;

void initFrame() {
    glClearColor(0.07f, 0.13f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    

    //glPixelZoom(PIXEL_ZOOM_X, PIXEL_ZOOM_Y);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
}

int index = 0;
void renderFrame() {
    //glClear(GL_COLOR_BUFFER_BIT);

    // Will get back 144 * 160 16bits per pixel
    uint8_t* frame = machine->getFrame();

    int block = 81;

    float x = (float)(index % 160) / 80 - 1;
    float y = (float)((index / 160) % 144) / 72 - 1;

    glRasterPos2f(x, y);

    if (index - block >= BUFFER_SIZE) {
        index = 0;
    }

    for (int i = 0; i < block; i += 3) {
        if (index % 2) {
            buffer[i] = 0xFF;
            buffer[i + 1] = 0x00;
            buffer[i + 2] = 0x00;
        }
        else {
            buffer[i] = 0x00;
            buffer[i + 1] = 0x00;
            buffer[i + 2] = 0xFF;
        }
    }

    index += 1;

    glDrawPixels(9, 9, GL_RGB, GL_UNSIGNED_BYTE, &buffer);

    //int block = 96 * 8;

    //if (index - block >= BUFFER_SIZE) {
    //    index = 0;
    //}

    //for (int i = 0; i < block; i++) {
    //    buffer[index] += (index++);
    //}

    //for (int i = 0; i < 10000; i++) {
    //    buffer[i] += i;
    //}

    //buffer = machine->getFrame();
    // tranlate buffer from hex vals to opengl buffer
    //glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
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
