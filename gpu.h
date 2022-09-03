#pragma once

#include <chrono>
#include "machine.h"

class Machine;

class GPU {
public:

    /*
     * Accepts pixels
     */
    class GameboyScreen {
        /*
         * Place a pixel into the screen
         * @param x X coordinate
         * @param y Y coordinate
         * @param color Color in RGB888 format
         */
        virtual void putPixel(int x, int y, int color) = 0;

        /*
         * Update the screen
         */
        virtual void update() = 0;
    };

    /*
     * @param machine Machine running
     * @param cgb True for gameboy color mode
     * @param compatibility True for monochrome compatibility mode on gameboy color
     */
    GPU(Machine*, bool , bool );

    /*
     * Needs to be called after constructor
     */
    void initState();

    static const int MS_BETWEEN_VBLANKS = (144 * (51 + 20 + 43)) * 1000 / (1 << 20);
    static const int WAIT_THRESHOLD = 4;

    static const int VRAM_SIZE = 0x2000;
    static const int SCREEN_HEIGHT = 144;
    static const int SCREEN_WIDTH = 160;
    const int grayPalette[4] = {
            0x00ffffff,
            0x00aaaaaa,
            0x00555555,
            0x00000000
    };

    int bgPalColor[4 * 8], obPalColor[4 * 8];

    uint8_t* vram;

    //GameboyScreen screen = null;
private:
    class SpriteAttrib {
    public:
        int x, y, pattern;
        bool priority, yFlip, xFlip, usePal1;

        // CGB only
        bool useVramBank1;
        int cgbPalette;
    };

    /**
     * For now, just update the screen if there is one
     */
    void doDraw();

    int line; // Current line being scanned
    int windowLine;
    int mode = 2; // 0 - hblank, 1 - vblank, 2 - OAM, 3 - VRAM
    int modeCycles; // Cycles spent on this line
    Machine* machine;

    bool lcdOn, windowMapHigh, windowOn, bgTileHigh, bgMapHigh, tallSprites, spritesOn, bgOn;
    bool oamInt, vblankInt, hblankInt, lycInt, lycCoincidence;
    int lyc;
    int scrollX, scrollY, windowX, windowY;
    int bgPal[4], ob0Pal[4], ob1Pal[4];

    //// CGB Only
    int bgPalIndex, obPalIndex;
    bool bgPalIncrement, obPalIncrement;
    //// End CGB Only

    SpriteAttrib* attribs = new SpriteAttrib[40];
    int spriteOrder[40];
    const bool* occluded = new bool[SCREEN_WIDTH];

    int zBuf[SCREEN_WIDTH * SCREEN_HEIGHT];

    uint64_t lastVBlank;

    bool cgb;
    bool compatibility;
    int vramBank; // CGB only
    bool oamPosOrder;
};
