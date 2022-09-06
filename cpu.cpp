#include "cpu.h"

typedef void (CPU::*functionPointer)();
typedef void (CPU::* functionPointer2)(uint8_t, uint8_t);

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->registers = new Registers();
    this->PC = 0;
    this->SP = 0;
    //execute(1);
    AddressBus[0] = 0x78;
    this->step();
}

void CPU::step() {
    uint8_t instruction = this->AddressBus[this->PC];
    execute(instruction);
    std::cout << "In Step";
    ;
}

void CPU::execute(uint8_t instruction) {
    //(this->*OpCodeMethods[CPU::OpCodes::ADD])();
    uint8_t opCode = instruction & 0xC0; // look at two most significant bits
    //(this->*OpCodeMethods[opCode])();
    
    void (CPU::*p1)(uint8_t, uint8_t);
    p1 = &CPU::LD;
    void* p2 = &p1;
    (this->*p1)(1, 1);
    (this->*(*(functionPointer2 *)p2))(2,2);

}

void CPU::INC() {
    std::cout << "In Inc";
}

void CPU::LD(uint8_t r1, uint8_t r2) {
    std::cout << "In Load" << int(r1) << int(r2) << std::endl;
}

void CPU::ADD() {
    std::cout << "In Add";
}

std::map<uint8_t, functionPointer> CPU::OpCodeMethods = { 
    {0x00, &CPU::ADD},
    //{0x40, &CPU::LD}
};
