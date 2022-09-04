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
    (this->*OpCodeMethods[CPU::OpCodes::ADD])();
}

std::map<CPU::OpCodes, void (CPU::*)()> CPU::OpCodeMethods = { {CPU::OpCodes::ADD, &CPU::hello} };
