#include "gpu.h"

GPU::GPU(Machine* machine, bool cgb, bool compatibility) {
    this->machine = machine;
    this->cgb = cgb;
    this->compatibility = compatibility;
    this->vram = new uint8_t[cgb ? (VRAM_SIZE * 2) : VRAM_SIZE];
    for (int i = 0; i < 40; i++) {
        spriteOrder[i] = i;
    }
    // Get current time in milliseconds
    this->lastVBlank = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void GPU::initState() {
    this->mode = 3; // Is this even really right?
    this->lycCoincidence = true;
}

void GPU::doDraw() {
    //if (screen != null) {
    //    screen.update();
    //}
}
