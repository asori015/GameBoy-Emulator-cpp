#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) {
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->VBLANK = false;
    this->state_ = State::Mode2;
    this->clock_ = 0;

    this->SCY = &addressBus[0xFF42];
    this->SCX = &addressBus[0xFF43];
}

void GPU::step() {
    // if LCD is on
    if (addressBus_[LCDC] & 0x80) {
        switch (state_)
        {
        case State::Mode0: // H-Blank
            if (clock_ >= 456) {
                addressBus_[LY] = (addressBus_[LY] + 1) % 154;
                if (addressBus_[LY] >= 144) {
                    state_ = State::Mode1;
                    // Transition into V-BLANK
                    // For now, not emulating cycle per cycle GPU
                    // Render the *entire* frame in one go
                    // Temporary soluton
                    renderFrame();
                    //printf("0x%x 0x%x\n", *SCX, *SCY);
                }
                else {
                    state_ = State::Mode2;
                }
                clock_ = -1;
            }
            break;
        case State::Mode1: // V-Blank
            if (clock_ >= 456) {
                addressBus_[LY] = (addressBus_[LY] + 1) % 154;
                //printf("%d\n", addressBus_[LY]);
                if (addressBus_[LY] == 0) {
                    state_ = State::Mode2;
                    printf("1 ");
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
                state_ = State::Mode0;
            }
            break;
        default:
            break;
        }

        //printf("%d", state_);

        clock_++;
    }
}

void GPU::mode3() {
    // STEP 1: Get Tile
    // if lcdc.3 = 1 and cur x outside "window"
    //    use tilemap2
    // else if lcdc.6 = 1 and cur x inside "window"
    //    use tilemap2
    // else
    //    use tilemap1
    //
    // 
}

// This function redundant and might be removed
bool GPU::getVBLANK() {
    return VBLANK;
}

// This is a temporary function that renders an entire LCD frame
void GPU::renderFrame() {
    // This is a TEMPORARY solution and there
    // is room for optimization and refactoring
    for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 160; j++) {
            int x = (*SCX + j) % 255;
            int y = (*SCY + i) % 255;
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

            frame_[(i * 160) + j] = color;
        }
    }

}

void GPU::renderTile(int tileIndex, int frameIndex, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint16_t VRAM_Pointer = (tileIndex * 16) + VRAM;
    for (int i = y1; i < y2; i++) {
        uint8_t lBits = addressBus_[VRAM_Pointer + (i * 2)];
        uint8_t hBits = addressBus_[VRAM_Pointer + (i * 2) + 1];
        uint8_t mask = 0x80;

        for (int j = x1; j < x2; j++) {
            //int frameIndex = frameIndex + (i * 160) + j;
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

            //frame_[frameIndex] = color;

            mask = mask >> 1;
        }
    }
}

void GPU::renderLine(uint8_t index, uint8_t y, uint8_t x1, uint8_t x2) {
    ;
}

void GPU::renderTile(uint8_t index, uint16_t* frame_, int position) {
    uint16_t VRAM_Pointer = (index * 16) + VRAM;
    for (int i = 0; i < 8; i++) {
        uint8_t lBits = addressBus_[VRAM_Pointer + (i * 2)];
        uint8_t hBits = addressBus_[VRAM_Pointer + (i * 2) + 1];
        uint8_t mask = 0x80;
        for (int j = 0; j < 8; j++) {
            int frameIndex = position + (i * 160) + j;
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

            frame_[frameIndex] = color;

            mask = mask >> 1;
        }
    }
}
