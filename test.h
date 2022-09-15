#include "cpu.h"

class Test {
public:
    Test();
    void runTests();
private:
    struct testCase {
        uint16_t AF;
        uint16_t BC;
        uint16_t DE;
        uint16_t HL;
        uint16_t SP;
        int instuctionCount;
    };

    void testLD();
    void testADD();
    bool compare(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    CPU* cpu;
    //const uint8_t testRoutine[3] = { 0x11, 0x22, 0x33 };
};