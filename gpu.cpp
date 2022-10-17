#include "gpu.h"

GPU::GPU(Machine* machine, bool cgb, bool compatibility) {
    
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