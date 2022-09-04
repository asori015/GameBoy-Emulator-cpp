#include "cpu.h"

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->registers = new Registers();
    execute(1);
}

void CPU::hello() {
    std::cout << "hello";
}

void test() {
    std::cout << "test";
}

void CPU::execute(uint8_t instruction) {
    (this->*OpCodeMethods[1])();
}

std::map<int, void (CPU::*)()> CPU::OpCodeMethods = { {1, &CPU::hello} };
