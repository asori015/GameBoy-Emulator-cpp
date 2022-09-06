#include "cpu.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->registers = new Registers();
    this->PC = 0;
    this->SP = 0;
    AddressBus[0] = 0x46;
    registers->REGS[4] = 0xff;
    registers->REGS[5] = 0xff;
    this->step();
}

void CPU::step() {
    uint8_t instruction = this->AddressBus[this->PC];
    execute(instruction);
    std::cout << "In Step";
}

void CPU::execute(uint8_t instruction) {
    uint8_t opCode = instruction & 0xC0; // look at two most significant bits
    (this->*OpCodeMethods[opCode])(instruction);
}

void CPU::INC(uint8_t instruction) {
    std::cout << "In Inc";
    auto f = [](uint8_t a, uint8_t b) -> int { return a + b; };
}

void CPU::LD(uint8_t instruction) {
    if ((instruction & 0xC0) == 0x40) {
        uint8_t r1 = instruction & 0x07;
        uint8_t r2 = (instruction & 0x38) >> 3;
        if (r1 == 0x06) {
            if (r2 == 0x06) {
                // HALT INSTRUCTION
            }
            else {
                this->registers->REGS[r2] = this->AddressBus[this->registers->HL()];
                //std::cout << "In Load: " << int(this->registers->REGS[r2]) << " -> " << int(this->registers->HL()) << std::endl;
            }
        }
        else if (r2 == 0x06) {
            this->AddressBus[this->registers->HL()] = this->registers->REGS[r1];
            //std::cout << "In Load: " << int(this->registers->REGS[r2]) << " -> " << int(r2) << std::endl;
        }
        else {
            this->registers->REGS[r2] = this->registers->REGS[r1];
            //std::cout << "In Load: " << int(r1) << " -> " << int(r2) << std::endl;
        }
    }
}

void CPU::ADD(uint8_t instruction) {
    std::cout << "In Add";
}

std::map<uint8_t, CPU::functionPointer> CPU::OpCodeMethods = { 
    {0x00, &CPU::ADD},
    {0x40, &CPU::LD},
    {0x01, &CPU::INC}
};
