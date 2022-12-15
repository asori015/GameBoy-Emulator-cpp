#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) : 
    LCDC(&addressBus[0xFF40]),
    STAT(&addressBus[0xFF41]),
    SCY(&addressBus[0xFF42]),
    SCX(&addressBus[0xFF43]),
    LY(&addressBus[0xFF44])
{
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->VBLANK = false;
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
    for (int j = 0; j < 160; j++) {
        int x = (*SCX + j) % 255;
        int y = (*SCY + *LY) % 255;
        int tileIndex = (y / 8) * 32 + (x / 8);
        int VRAM_Pointer = VRAM + (addressBus_[TILE_MAP1 + tileIndex] * 16);

        uint8_t lBits = addressBus_[VRAM_Pointer + ((y % 8) * 2)];
        uint8_t hBits = addressBus_[VRAM_Pointer + ((y % 8) * 2) + 1];
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

        frame_[(*LY * 160) + j] = color;
    }
}
