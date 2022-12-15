#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) : 
    VRAM_1(&addressBus[0x8000]),
    VRAM_2(&addressBus[0x9000]),
    TILE_MAP1(&addressBus[0x9800]),
    TILE_MAP2(&addressBus[0x9C00]),
    OAM(&addressBus[0xFE00]),
    LCDC(&addressBus[0xFF40]),
    STAT(&addressBus[0xFF41]),
    SCY(&addressBus[0xFF42]),
    SCX(&addressBus[0xFF43]),
    LY(&addressBus[0xFF44])
{
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->state_ = State::Mode2;
    this->clock_ = 0;
}

void GPU::step() {
    // if LCD is on
    if (*LCDC & 0x80) {
        switch (state_)
        {
        case State::Mode0: // H-Blank
            if (clock_ >= 456) {
                *LY = (*LY + 1) % 154;
                if (*LY >= 144) {
                    state_ = State::Mode1;
                }
                else {
                    state_ = State::Mode2;
                }
                clock_ = -1;
            }
            break;
        case State::Mode1: // V-Blank
            if (clock_ >= 456) {
                *LY = (*LY + 1) % 154;
                if (*LY == 0) {
                    state_ = State::Mode2;
                    //printf("1 ");
                }
                clock_ = -1;
            }
            break;
        case State::Mode2: // OAM Scan
            if (clock_ >= 80) {
                state_ = State::Mode3;
            }
            break;
        case State::Mode3: // Drawing Pixels
            if (clock_ >= 172) {
                // Transition into H-BLANK
                state_ = State::Mode0;
                renderLine();
            }
            break;
        default:
            break;
        }

        clock_++;
    }
}

void GPU::renderLine() {
    clearLine();
    if (*LCDC & 0x01) {
        renderBGLine();
    }
}

void GPU::renderBGLine() {
    for (int i = 0; i < 160; i++) {
        int x = (*SCX + i) % 255;
        int y = (*SCY + *LY) % 255;
        int tileIndex = (y / 8) * 32 + (x / 8);
        const uint8_t* VRAM_Pointer;

        if (*LCDC & 0x10) {
            if (*LCDC & 0x08) {
                VRAM_Pointer = VRAM_1 + (*(TILE_MAP2 + tileIndex) * 16);
            }
            else {
                VRAM_Pointer = VRAM_1 + (*(TILE_MAP1 + tileIndex) * 16);
            }
        }
        else {
            if (*LCDC & 0x08) {
                VRAM_Pointer = VRAM_2 + ((int8_t)(*(TILE_MAP2 + tileIndex)) * 16);
            }
            else {
                VRAM_Pointer = VRAM_2 + ((int8_t)(*(TILE_MAP1 + tileIndex)) * 16);
            }
        }
        

        uint8_t lBits = *(VRAM_Pointer + ((y % 8) * 2));
        uint8_t hBits = *(VRAM_Pointer + ((y % 8) * 2) + 1);
        uint8_t mask = 0x80 >> (x % 8);
        uint16_t color;

        if (hBits & mask) {
            if (lBits & mask) {
                color = 0x0000;
            }
            else {
                color = 0x294A;
            }
        }
        else {
            if (lBits & mask) {
                color = 0x56B5;
            }
            else {
                color = 0xFFFF;
            }
        }

        frame_[(*LY * 160) + i] = color;
    }
}

void GPU::clearLine() {
    for (int i = 0; i < 160; i++) {
        frame_[(*LY * 160) + i] = 0xFFFF;
    }
}
