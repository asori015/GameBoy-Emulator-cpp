#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) {
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->VBLANK = false;
    this->state_ = State::Mode2;
    this->clock_ = 0;
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
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 20; j++) {
            uint8_t tileIndex = addressBus_[TILE_MAP1 + (i * 32) + j];
            renderTile(tileIndex, frame_, (i * 160 * 8) + (j * 8));
        }
    }

    return;
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
                    color = 0x5555;
                }
            }
            else {
                if (lBits & mask) {
                    color = 0xAAAA;
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

//init
//const int VRAM = 0x8000;
//const int TILE_MAP1 = 0x9800;
//const int TILE_MAP1_END = 0x9BFF;
//for (int i = 0; i < 18; i++) {
//    for (int j = 0; j < 20; j++) {
//        uint8_t tileIndex = machine->cpu->addressBus_[TILE_MAP1 + (i * 32) + j];
//        renderTile(machine, tileIndex, buffer, (i * 160 * 8 * 3) + (j * 8 * 3));
//    }
//}
//
//for (int i = 0; i < 144 / 2; i++) {
//    for (int j = 0; j < 160 * 3; j++) {
//        uint8_t temp = buffer[(i * 160 * 3) + j];
//        buffer[(i * 160 * 3) + j] = buffer[((144 - i - 1) * 160 * 3) + j];
//        buffer[((144 - i - 1) * 160 * 3) + j] = temp;
//    }
//}

