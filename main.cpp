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

void renderTile(Machine* machine, uint8_t index, uint8_t* buffer, int position) {
    uint16_t VRAM_Address = (index * 16) + 0x8000;
    for (int i = 0; i < 8; i++) {
        uint8_t lBits = machine->cpu->addressBus_[VRAM_Address + (i * 2)];
        uint8_t hBits = machine->cpu->addressBus_[VRAM_Address + (i * 2) + 1];
        uint8_t mask = 0x80;
        for (int j = 0; j < 8; j++) {
            int bufferIndex = position + (i * 160 * 3) + (j * 3);
            uint8_t color;

            if (hBits & mask) {
                if (lBits & mask) {
                    color = 0x00;
                }
                else {
                    color = 0x55;
                }
            }
            else {
                if (lBits & mask) {
                    color = 0xAA;
                }
                else {
                    color = 0xFF;
                }
            }

            buffer[bufferIndex] = color;
            buffer[bufferIndex + 1] = color;
            buffer[bufferIndex + 2] = color;

            mask = mask >> 1;
        }
    }
}

void initFrame() {
    glClearColor(0.07f, 0.13f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    

    glPixelZoom(PIXEL_ZOOM_X, PIXEL_ZOOM_Y);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
}

int index = 0;
void renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glutSwapBuffers();
}

void run(int argc, char** argv){
    // For testing and debugging purposes
    //Test* test = new Test();

    std::string ROMPath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    machine = new Machine(ROMPath, Machine::MachineModeEnum::GAMEBOY);

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
