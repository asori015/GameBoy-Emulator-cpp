#include "gpu.h"

GPU::GPU(Machine* machine, uint8_t* addressBus, uint16_t* frame) : 
    VRAM_1(&addressBus[0x8000]),
    VRAM_2(&addressBus[0x9000]),
    TILE_MAP1(&addressBus[0x9800]),
    TILE_MAP2(&addressBus[0x9C00]),
    OAM(&addressBus[0xFE00]),
    IF(&addressBus[0xFF0F]),
    LCDC(&addressBus[0xFF40]),
    STAT(&addressBus[0xFF41]),
    SCY(&addressBus[0xFF42]),
    SCX(&addressBus[0xFF43]),
    LY(&addressBus[0xFF44]),
    LYC(&addressBus[0xFF45]),
    DMA(&addressBus[0xFF46]),
    BGP(&addressBus[0xFF47]),
    OBP0(&addressBus[0xFF48]),
    OBP1(&addressBus[0xFF49]),
    WY(&addressBus[0xFF4A]),
    WX(&addressBus[0xFF4B])
{
    this->addressBus_ = addressBus;
    this->frame_ = frame;
    this->state_ = State::Mode2;
    this->clock_ = 0;
    this->windowLineCounter_ = 0;
}

void GPU::step() {
    // if LCD is on
    if (*LCDC & 0x80) {
        switch (state_)
        {
        case State::Mode0: // H-Blank
            if (clock_ >= 456) {
                if (*LY >= 143) {
                    state_ = State::Mode1;
                    *STAT &= 0xFC; // Set mode on STAT register
                    *STAT |= 0x01;
                    *LY += 1;
                    *IF |= 0x01; // Set V-BLANK interupt
                    if (*STAT & 0x10) { *IF |= 0x02; } // Check if STAT interupt enable, request interupt
                    windowLineCounter_ = 0;
                }
                else {
                    state_ = State::Mode2;
                    *STAT &= 0xFC; // Set mode on STAT register
                    *STAT |= 0x02;
                    *LY += 1;
                    if (*STAT & 0x20) { *IF |= 0x02; } // Check if STAT interupt enable, request interupt
                }
                clock_ = -1;
            }
            break;
        case State::Mode1: // V-Blank
            if (clock_ >= 456) {
                *LY += 1;
                if (*LY == 0x9A) {
                    state_ = State::Mode2;
                    *STAT &= 0xFC; // Set mode on STAT register
                    *STAT |= 0x02;
                    *LY = 0;
                    if (*STAT & 0x20) { *IF |= 0x02; } // Check if STAT interupt enable, request interupt
                    printf("1 ");
                }
                clock_ = -1;
            }
            break;
        case State::Mode2: // OAM Scan
            if (clock_ >= 80) {
                state_ = State::Mode3;
                *STAT |= 0X03; // Set mode on STAT register

            }
            break;
        case State::Mode3: // Drawing Pixels
            if (clock_ >= 172) {
                // Transition into H-BLANK
                state_ = State::Mode0;
                *STAT &= 0XFC; // Set mode on STAT register
                if (*STAT & 0x08) { *IF |= 0x02; } // Check if STAT interupt enable, request interupt
                renderLine();
            }
            break;
        default:
            break;
        }

        if (*LY == *LYC) {
            if ((*STAT & 0x04) == 0) {
                if (*STAT & 0x40) {
                    *IF |= 0x02;
                }
                *STAT |= 0x04;
            }
        }
        else {
            *STAT &= 0xFB;
        }

        clock_++;
    }
}

void GPU::renderLine() {
    //clearLine();
    if (*LCDC & 0x01) {
        renderBackgroundLine();
    }
    if (*LCDC & 0x20) {
        renderWindowLine();
    }
    if (*LCDC & 0x02) {
        renderObjectLine();
    }
}

void GPU::renderBackgroundLine() {
    for (int i = 0; i < 160; i++) {
        int x = (*SCX + i) % 256;
        int y = (*SCY + *LY) % 256;
        int tileIndex = ((y / 8) * 32) + (x / 8);
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
                color = colorValues[(*BGP & 0xC0) >> 6];
                bgDotVals[i] = 3;
            }
            else {
                color = colorValues[(*BGP & 0x30) >> 4];
                bgDotVals[i] = 2;
            }
        }
        else {
            if (lBits & mask) {
                color = colorValues[(*BGP & 0x0C) >> 2];
                bgDotVals[i] = 1;
            }
            else {
                color = colorValues[(*BGP & 0x03)];
                bgDotVals[i] = 0;
            }
        }

        frame_[(*LY * 160) + i] = color;
    }
}

void GPU::renderWindowLine() {
    if (*LY >= *WY) {
        for (int i = *WX - 7; i < 160; i++) {
            int x = i + 7 - *WX;
            int y = windowLineCounter_;
            int tileIndex = ((y / 8) * 32) + (x / 8);
            const uint8_t* VRAM_Pointer;

            if (*LCDC & 0x40) {
                VRAM_Pointer = VRAM_1 + (*(TILE_MAP2 + tileIndex) * 16);
            }
            else {
                VRAM_Pointer = VRAM_1 + (*(TILE_MAP1 + tileIndex) * 16);
            }

            uint8_t lBits = *(VRAM_Pointer + ((y % 8) * 2));
            uint8_t hBits = *(VRAM_Pointer + ((y % 8) * 2) + 1);
            uint8_t mask = 0x80 >> (x % 8);
            uint16_t color;

            if (hBits & mask) {
                if (lBits & mask) {
                    color = colorValues[(*BGP & 0xC0) >> 6];
                    bgDotVals[i] = 3;
                }
                else {
                    color = colorValues[(*BGP & 0x30) >> 4];
                    bgDotVals[i] = 2;
                }
            }
            else {
                if (lBits & mask) {
                    color = colorValues[(*BGP & 0x0C) >> 2];
                    bgDotVals[i] = 1;
                }
                else {
                    color = colorValues[(*BGP & 0x03)];
                    bgDotVals[i] = 0;
                }
            }

            frame_[(*LY * 160) + i] = color;
        }
        if (*WX <= 166) {
            windowLineCounter_++;
        }
    }
}

void GPU::renderObjectLine() {
    int yCount = 0;
    for (int i = 0; i < 40; i++) {
        int y = *(OAM + (i * 4)) - *LY;
        int x = *(OAM + (i * 4) + 1) - 8;
        uint8_t attributes = *(OAM + (i * 4) + 3);
        const uint8_t* VRAM_Pointer;

        if (*LCDC & 0x04) {
            if (y < 1 || y > 16) {
                continue;
            }
            if (y < 9) {
                if (attributes & 0x40) {
                    VRAM_Pointer = VRAM_1 + ((*(OAM + (i * 4) + 2) & 0xFE) * 16);
                }
                else {
                    VRAM_Pointer = VRAM_1 + ((*(OAM + (i * 4) + 2) | 0x01) * 16);
                }
            }
            else {
                if (attributes & 0x40) {
                    VRAM_Pointer = VRAM_1 + ((*(OAM + (i * 4) + 2) | 0x01) * 16);
                }
                else {
                    VRAM_Pointer = VRAM_1 + ((*(OAM + (i * 4) + 2) & 0xFE) * 16);
                }
            }
        }
        else {
            if (y < 9 || y > 16) {
                continue;
            }
            VRAM_Pointer = VRAM_1 + (*(OAM + (i * 4) + 2) * 16);
        }

        
        if (attributes & 0x40) {
            y = y - 1;
        }
        else {
            y = 16 - y;
        }

        uint8_t lBits = *(VRAM_Pointer + ((y % 8) * 2));
        uint8_t hBits = *(VRAM_Pointer + ((y % 8) * 2) + 1);
        uint8_t mask;

        if (attributes & 0x20) {
            mask = 0x01;
        }
        else {
            mask = 0x80;
        }

        for (int j = 0; j < 8; j++) {
            if (x >= 0 || x < 160) {
                uint16_t color;
                const uint8_t* pallete;

                if (attributes & 0x10) {
                    pallete = OBP1;
                }
                else {
                    pallete = OBP0;
                }

                //uint8_t colorIndex = 0;

                if (hBits & mask) {
                    if (lBits & mask) {
                        color = colorValues[(*pallete & 0xC0) >> 6];
                        //color = 0x7C00;
                    }
                    else {
                        color = colorValues[(*pallete & 0x30) >> 4];
                        //color = 0x03E0;
                    }
                }
                else {
                    if (lBits & mask) {
                        color = colorValues[(*pallete & 0x0C) >> 2];
                        //color = 0x7FE0;
                    }
                    else {
                        if (attributes & 0x20) {
                            mask = mask << 1;
                        }
                        else {
                            mask = mask >> 1;
                        }
                        x++;
                        continue;
                        //color = 0x7C1F;
                        //color = colorValues[(*pallete & 0x03)];
                    }
                }
                if ((attributes & 0x80)) {
                    if (bgDotVals[x] == 0) {
                        frame_[(*LY * 160) + x] = color;
                    }
                }
                else {
                    frame_[(*LY * 160) + x] = color;
                }
            }

            if (attributes & 0x20) {
                mask = mask << 1;
            }
            else {
                mask = mask >> 1;
            }

            x++;
        }

        yCount += 1;
        if (yCount == 10) {
            break;
        }
    }
}

void GPU::clearLine() {
    for (int i = 0; i < 160; i++) {
        frame_[(*LY * 160) + i] = 0xFFFF;
    }
}
