#include "cpu.h"

class Test {
public:
    Test();
    void runTests();
private:
    void testLD();
    void testADD();
    bool compare(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    CPU* cpu;
    //const uint8_t testRoutine[3] = { 0x11, 0x22, 0x33 };
};