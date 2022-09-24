#include <iostream>
#include <fstream>
// #include "olcConsoleGameEngine.h"
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

//class Screen{
//    public:
//     Screen(Machine* x);
//     Keypad keypad;
//    private:
//     Machine* x;
//};
//
//Screen::Screen(Machine* x){
//    this->x = x;
//}


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
                    color = 0x08;
                }
            }
            else {
                if (lBits & mask) {
                    color = 0x0F;
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

    // Temporary, will be moved to GPU
    //for index in tile map 
    //
    //
    const int VRAM = 0x8000;
    const int TILE_MAP1 = 0x9800;
    const int TILE_MAP1_END = 0x9BFF;
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 20; j++) {
            uint8_t tileIndex = machine->cpu->addressBus_[TILE_MAP1 + (i * 32) + j];
            renderTile(machine, tileIndex, buffer, (i * 160 * 8 * 3) + (j * 8 * 3));
        }
    }

    for (int i = 0; i < 144 / 2; i++) {
        for (int j = 0; j < 160 * 3; j++) {
            uint8_t temp = buffer[(i * 160 * 3) + j];
            buffer[(i * 160 * 3) + j] = buffer[((144 - i - 1) * 160 * 3) + j];
            buffer[((144 - i - 1) * 160 * 3) + j] = temp;
        }
    }
    

    glPixelZoom(PIXEL_ZOOM_X, PIXEL_ZOOM_Y);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    //glutSwapBuffers();
}

int index = 0;
void renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    //char* cbuffer = buffer;

    //glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, cbuffer + state / 500);
    //buffer[index] = 0xFF;//!buffer[index];
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glutSwapBuffers();
    //glutPostRedisplay();
    //index = (index + 1) % BUFFER_SIZE;
    //std::cout << state / 500 << std::endl;
}

void run(int argc, char** argv){
    // For testing and debugging purposes
    //Test* test = new Test();

    std::string ROMPath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    machine = new Machine(ROMPath, Machine::MachineModeEnum::GAMEBOY);
    //Screen* screen = new Screen(machine);
    //screen->keypad = machine->getKeypad();

    //for (uint16_t i = 0x9800; i < 0x9C00; i++) {
    //    if (i % 32 == 0) {
    //        printf("\n");
    //    }
    //    printf("%02X ", machine->cpu->addressBus_[i]);
    //    //printf("%#4X: %02X\n", i, machine->cpu->addressBus_[i]);
    //}

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

    /////////////////////////////////////

    //Machine machine = new Machine(new File(ROMPath), Machine.MachineMode.GAMEBOY_COLOR);
    //Screen screen = new Screen(machine);
    //screen.keypad = machine.getKeypad();
    //machine.attachScreen(screen);
    //screen.makeContainer();
    //PcSpeaker speaker = new PcSpeaker();
    //machine.attachSpeaker(speaker);
    //int[] pal = machine.getDmgPalette();
    //// Totally arbitrary palette
    //pal[0] = 0x0000ffff;
    //pal[1] = 0x0020b010;
    //pal[2] = 0x00400000;
    //pal[3] = 0x00000000;
    //while (screen.isOpen()) {
    //    machine.cycle();
    //}
    //try {
    //    machine.saveExternal();
    //}
    //catch (Exception e) {
    //    e.printStackTrace();
    //}
    //System.out.println("Stopped");
}

int main(int argc, char** argv){
    run(argc, argv);
    return 0;
}
