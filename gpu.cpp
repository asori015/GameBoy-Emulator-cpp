#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) {
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->VBLANK = false;
    this->state_ = 2;
    this->clock_ = 0;
}

void GPU::step() {
    // if LCD is on
    if (addressBus_[LCDC] & 0x80) {
        if (clock_ >= 65664) {
            // Mode 1, V-BLANK
        }
        else if(clock_ % 456 < 80){
            // Mode 2, OAM Scan
        }
        else if (clock_ % 456 < 200) {
            // Mode 3, Drawing Pixels
        }
        else {
            // Mode 0, H-Blank
        }

        addressBus_[LY] = clock_ / 456;
        if (clock_ == 70224) {
            clock_ = 0;
            //printf("1 ");
        }
        else {
            clock_ += 1;
        }
    }
}

bool GPU::getVBLANK() {
    return VBLANK;
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

//void renderTile(Machine* machine, uint8_t index, uint8_t* buffer, int position) {
//    uint16_t VRAM_Address = (index * 16) + 0x8000;
//    for (int i = 0; i < 8; i++) {
//        uint8_t lBits = machine->cpu->addressBus_[VRAM_Address + (i * 2)];
//        uint8_t hBits = machine->cpu->addressBus_[VRAM_Address + (i * 2) + 1];
//        uint8_t mask = 0x80;
//        for (int j = 0; j < 8; j++) {
//            int bufferIndex = position + (i * 160 * 3) + (j * 3);
//            uint8_t color;
//
//            if (hBits & mask) {
//                if (lBits & mask) {
//                    color = 0x00;
//                }
//                else {
//                    color = 0x55;
//                }
//            }
//            else {
//                if (lBits & mask) {
//                    color = 0xAA;
//                }
//                else {
//                    color = 0xFF;
//                }
//            }
//
//            buffer[bufferIndex] = color;
//            buffer[bufferIndex + 1] = color;
//            buffer[bufferIndex + 2] = color;
//
//            mask = mask >> 1;
//        }
//    }
//}