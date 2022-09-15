#include "test.h"

Test::Test() {
    this->runTests();
}

void Test::runTests() {
    this->testLD();
    //this->testADD();
    //this->cpu->loadBIOS(this->testRoutine, 1, 0);
    //;
}

void Test::testLD() {
    const int ARR_SIZE = 41;
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
    };

    const int NUM_TESTS = 6;
    testCase testCases[NUM_TESTS] = {
        {0x0100, 0x0203, 0x0405, 0x0607, 0x0000, 7},
        {0x0100, 0x0303, 0x0505, 0x0707, 0x0000, 3},
        {0x0100, 0x3E03, 0x0105, 0x0000, 0x0000, 5},
        {0x0100, 0xFF03, 0x0105, 0x0001, 0x0000, 3},
        {0xFF00, 0xFF03, 0x0105, 0x0000, 0x0000, 4},
        {0x4200, 0xFF88, 0x0105, 0x0000, 0x0000, 5}
    };

    this->cpu = new CPU(0);
    cpu->loadBIOS(LD_snippet, ARR_SIZE, 0);

    for (int i = 0; i < NUM_TESTS; i++) {
        for (int j = 0; j < testCases[i].instuctionCount; j++) {
            cpu->step();
        }
        if (!this->compare(testCases[i].AF, testCases[i].BC, testCases[i].DE, testCases[i].HL, testCases[i].SP)) {
            std::cout << "LD Test Failed" << std::endl;
            return;
        }
    }

    std::cout << "LD Test Passed!" << std::endl;

    delete cpu;
}

void Test::testADD() {
    const int ARR_SIZE = 5;
    const uint8_t ADD_snippet[ARR_SIZE] = {
        0x3E, // LD A d8
        0x3A,
        0x06, // LD B d8
        0xC6,
        0x80  // ADD A B
    };

    this->cpu = new CPU(0);
    cpu->loadBIOS(ADD_snippet, ARR_SIZE, 0);
    for (int i = 0; i < ARR_SIZE; i++) {
        cpu->step();
    }
    //cpu->printRegs();
}

bool Test::compare(uint16_t AF, uint16_t BC, uint16_t DE, uint16_t HL, uint16_t SP) {
    return AF == cpu->getAF() && BC == cpu->getBC() && DE == cpu->getDE() && HL == cpu->getHL() && SP == cpu->getSP();
}
