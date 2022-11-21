#include "test.h"

Test::Test() {
    this->runTests();
}

void Test::runTests() {
    this->testLD();
    this->testArithmetic();
}

void Test::testLD() {
    const int ARR_SIZE = 66;
    const uint8_t LD_snippet[ARR_SIZE] = {
        0x3E, //  1; LD A d8
        0x01,
        0x06, //  2; LD B d8
        0x02,
        0x0E, //  3; LD C d8
        0x03,
        0x16, //  4; LD D d8
        0x04,
        0x1E, //  5; LD E d8
        0x05,
        0x26, //  6; LD H d8
        0x06,
        0x2E, //  7; LD L d8;    1st compare
        0x07,
        0x41, //  8; LD B C
        0x53, //  9; LD D E
        0x65, // 10; LD H L;     2nd compare
        0x26, // 11; LD H d8
        0x00,
        0x2E, // 12; LD L d8;
        0x00,
        0x46, // 13; LD B, (HL)
        0x77, // 14; LD (HL) A
        0x56, // 15; LD D, (HL)  3rd compare
        0x6F, // 16; LD L, A
        0x36, // 17; LD (HL), d8
        0xFF,
        0x46, // 18; LD B, (HL)  4th compare
        0x32, // 19; LD (HLD), A
        0x32, // 20; LD (HLD), A
        0x70, // 21; LD (HL), B
        0x2A, // 22; LD A, (HLI) 5th compare
        0x3E, // 23, LD A, d8;
        0x42, //
        0x0E, // 24; LD C, d8;
        0x88, //
        0xE2, // 25; LD (C), A
        0x78, // 26; LD A, B
        0xFA, // 27; A,(a16)     6th compare
        0x88, //
        0xFF, //
        0x01, // 28; LD BC, d16
        0x22, //
        0x11, //
        0x31, // 29; LD SP, d16
        0xF8, //
        0xFF, //
        0x08, // 30; LD (a16),SP
        0x00, //
        0x00, //
        0x21, // 31; LD HL, d16
        0x00, //
        0x00, //
        0x2A, // 32; LD A,(HL+)
        0x5F, // 33; LD E, A
        0x2A, // 34; LD A,(HL+)
        0x57, // 35; LD D, A     7th compare
        0x31, // 36; LD SP, d16
        0xFF, //
        0xBF, //
        0xF8, // 37; LD HL,SP+r8
        0x80, //
        0xF8, // 38; LD HL,SP+r8
        0x7F, //
        0xC5, // 39 PUSH BC
        0XD1, // 40 POP DE
    };

    const int NUM_TESTS = 8;
    testCase testCases[NUM_TESTS] = {
        {0x0100, 0x0203, 0x0405, 0x0607, 0x0000, 7},
        {0x0100, 0x0303, 0x0505, 0x0707, 0x0000, 3},
        {0x0100, 0x3E03, 0x0105, 0x0000, 0x0000, 5},
        {0x0100, 0xFF03, 0x0105, 0x0001, 0x0000, 3},
        {0xFF00, 0xFF03, 0x0105, 0x0000, 0x0000, 4},
        {0x4200, 0xFF88, 0x0105, 0x0000, 0x0000, 5},
        {0xFF00, 0x1122, 0xFFF8, 0x0002, 0xFFF8, 8},
        {0xFF20, 0x1122, 0x1122, 0xC07E, 0xBFFF, 5}
    };

    uint8_t* addressBus = new uint8_t[0xFFFF]{ 0 };
    this->cpu = new CPU(0, addressBus);
    cpu->loadBIOS(LD_snippet, ARR_SIZE, 0);

    for (int i = 0; i < NUM_TESTS; i++) {
        for (int j = 0; j < testCases[i].instuctionCount; j++) {
            cpu->step();
        }
        if (!this->compare(testCases[i].AF, testCases[i].BC, testCases[i].DE, testCases[i].HL, testCases[i].SP)) {
            std::cout << "LD Test " << i << " Failed" << std::endl;
            return;
        }
        else {
            std::cout << "LD Test " << i << " Passed!" << std::endl;
        }
    }

    std::cout << "LD Tests Passed!" << std::endl;

    delete cpu;
}

void Test::testArithmetic() {
    const int ARR_SIZE = 66;
    const uint8_t ADD_snippet[ARR_SIZE] = {
        0x3E, //  1; LD A, d8
        0x3A, //
        0x06, //  2; LD B, d8
        0xC6, //
        0x80, //  3; ADD A, B;    1st compare
        0xAF, //  4; XOR A, A;    clearing flags
        0x3E, //  5; LD A, d8
        0x3C, //
        0xC6, //  6; ADD A, d8;   2nd compare
        0xFF, //
        0xAF, //  7; XOR A, A;    clearing flags
        0x36, //  8; LD (HL), d8
        0x12, //
        0x3E, //  9; LD A, d8
        0x3C, //
        0x86, // 10; ADD A, (HL); 3rd compare
        0x31, // 11; LD SP, d16
        0xF8, //
        0xFF, //
        0x3E, // 12; LD A, d8
        0xF0, // 
        0xC6, // 13; ADD A, d8;
        0xF1, //
        0xF5, // 14; PUSH AF
        0xF5, // 15; PUSH AF
        0x06, // 16; LD B, d8
        0x0F, //
        0x88, // 17; ADC A, B;    4th compare
        0xF1, // 18; POP AF
        0xCE, // 19; ADC A, d8    5th compare
        0x3B,
        0xF1, // 18; POP AF
        0x36, // 20; LD (HL), d8
        0x1E, //
        0x8E, // 21; ADC A, (HL)  6th compare
        0x3E, // 22; LD A, d8
        0x3E, // 
        0xB7, // 23; OR A, A;     clearing flags
        0xF5, // 24; PUSH AF
        0xF5, // 25; PUSH AF
        0x06, // 26; LD B, d8
        0x3E, //
        0x90, // 27; SUB A, B     7th compare
        0xF1, // 28; POP AF
        0xD6, // 29; SUB A, d8    8th compare
        0x0F,
        0xF1, // 30; POP AF
        0x36, // 31; LD (HL), d8
        0x40, //
        0x96, // 32; SUB A, (HL)  9th compare
        0x3E, // 33; LD A, d8
        0xF0, // 
        0xC6, // 34; ADD A, d8;
        0x4B, //
        0xF5, // 35; PUSH AF
        0xF5, // 36; PUSH AF
        0x06, // 37; LD B, d8
        0x2A, //
        0x98, // 38; SBC A, B;    10th compare
        0xF1, // 28; POP AF
        0xDE, // 29; SBC A, d8    11th compare
        0xFF,
        0xF1, // 30; POP AF
        0x36, // 31; LD (HL), d8
        0x4F, //
        0x9E, // 32; SBC A, (HL)  12th compare
    };

    const int NUM_TESTS = 12;
    testCase testCases[NUM_TESTS] = {
        {0x00B0, 0xC600, 0x0000, 0x0000, 0x0000, 3},
        {0x3B30, 0xC600, 0x0000, 0x0000, 0x0000, 3},
        {0x4E00, 0xC600, 0x0000, 0x0000, 0x0000, 4},
        {0xF120, 0x0F00, 0x0000, 0x0000, 0xFFF4, 7},
        {0x1D10, 0x0F00, 0x0000, 0x0000, 0xFFF6, 2},
        {0x00B0, 0x0F00, 0x0000, 0x0000, 0xFFF8, 3},
        {0x00C0, 0x3E00, 0x0000, 0x0000, 0xFFF4, 6},
        {0x2F60, 0x3E00, 0x0000, 0x0000, 0xFFF6, 2},
        {0xFE50, 0x3E00, 0x0000, 0x0000, 0xFFF8, 3},
        {0x1040, 0x2A00, 0x0000, 0x0000, 0xFFF4, 6},
        {0x3B70, 0x2A00, 0x0000, 0x0000, 0xFFF6, 2},
        {0xEB70, 0x2A00, 0x0000, 0x0000, 0xFFF8, 3},
    };

    uint8_t* addressBus = new uint8_t[0xFFFF]{ 0 };
    this->cpu = new CPU(0, addressBus);
    cpu->loadBIOS(ADD_snippet, ARR_SIZE, 0);

    for (int i = 0; i < NUM_TESTS; i++) {
        for (int j = 0; j < testCases[i].instuctionCount; j++) {
            cpu->step();
        }
        if (!this->compare(testCases[i].AF, testCases[i].BC, testCases[i].DE, testCases[i].HL, testCases[i].SP)) {
            std::cout << "Arithmetic Test " << i << " Failed" << std::endl;
            return;
        }
        else {
            std::cout << "Arithmetic Test " << i << " Passed!" << std::endl;
        }
    }

    std::cout << "Arithmetic Tests Passed!" << std::endl;

    delete cpu;
}

bool Test::compare(uint16_t AF, uint16_t BC, uint16_t DE, uint16_t HL, uint16_t SP) {
    return AF == cpu->getAF() && BC == cpu->getBC() && DE == cpu->getDE() && HL == cpu->getHL() && SP == cpu->getSP();
}
