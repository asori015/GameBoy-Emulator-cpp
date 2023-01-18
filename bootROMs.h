#pragma once

#include <cstdint>

class BootROMs {
    public:
     /*
     * The boot ROM from the original DMG gameboy
     */
     static const uint8_t BIOS_DMG[];

     /*
     * The boot ROM from the Gameboy Color
     */
     static const uint8_t BIOS_CGB[];
};
