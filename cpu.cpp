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

void CPU::Handle_00_Opcodes(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;
    if (r1 == 0x01) {
        switch (r2)
        {
        case 0x00:
            this->AddressBus[this->registers->getBC()] = this->registers->REGS[this->registers->A];
            break;
        case 0x01:
            this->registers->REGS[this->registers->A] = this->AddressBus[this->registers->getBC()];
            break;
        case 0x02:
            this->AddressBus[this->registers->getDE()] = this->registers->REGS[this->registers->A];
            break;
        case 0x03:
            this->registers->REGS[this->registers->A] = this->AddressBus[this->registers->getDE()];
            break;
        case 0x04:
            this->AddressBus[hl] = this->registers->REGS[this->registers->A];
            hl += 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x05:
            this->registers->REGS[this->registers->A] = this->AddressBus[hl];
            hl += 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x06:
            this->AddressBus[hl] = this->registers->REGS[this->registers->A];
            hl -= 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x07:
            this->registers->REGS[this->registers->A] = this->AddressBus[hl];
            hl -= 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        default:
            break;
        }
    }
    else if (r1 == 0x02) {
        uint16_t hl = this->registers->getHL();
        switch (r2)
        {
        case 0x00:
            this->AddressBus[this->registers->getBC()] = this->registers->REGS[this->registers->A];
            break;
        case 0x01:
            this->registers->REGS[this->registers->A] = this->AddressBus[this->registers->getBC()];
            break;
        case 0x02:
            this->AddressBus[this->registers->getDE()] = this->registers->REGS[this->registers->A];
            break;
        case 0x03:
            this->registers->REGS[this->registers->A] = this->AddressBus[this->registers->getDE()];
            break;
        case 0x04:
            this->AddressBus[hl] = this->registers->REGS[this->registers->A];
            hl += 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x05:
            this->registers->REGS[this->registers->A] = this->AddressBus[hl];
            hl += 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x06:
            this->AddressBus[hl] = this->registers->REGS[this->registers->A];
            hl -= 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        case 0x07:
            this->registers->REGS[this->registers->A] = this->AddressBus[hl];
            hl -= 1;
            this->registers->REGS[this->registers->H] = uint8_t(hl >> 8);
            this->registers->REGS[this->registers->L] = uint8_t(hl & 0x00FF);
            break;
        default:
            break;
        }
    }
    else if (r1 == 0x06) {
        if (r2 == 0x06) {
            this->AddressBus[this->registers->getHL()] = this->AddressBus[this->PC];
        }
        else {
            this->registers->REGS[r2] = this->AddressBus[this->PC];
        }
    }
}

void CPU::Handle_01_Opcodes(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;
    if (r1 == 0x06) {
        if (r2 == 0x06) {
            // HALT INSTRUCTION
        }
        else {
            this->registers->REGS[r2] = this->AddressBus[this->registers->getHL()];
            //std::cout << "In Load: " << int(this->registers->REGS[r2]) << " -> " << int(this->registers->HL()) << std::endl;
        }
    }
    else if (r2 == 0x06) {
        this->AddressBus[this->registers->getHL()] = this->registers->REGS[r1];
        //std::cout << "In Load: " << int(this->registers->REGS[r2]) << " -> " << int(r2) << std::endl;
    }
    else {
        this->registers->REGS[r2] = this->registers->REGS[r1];
        //std::cout << "In Load: " << int(r1) << " -> " << int(r2) << std::endl;
    }
}

void CPU::Handle_10_Opcodes(uint8_t instruction) {
    std::cout << "In Add";
}

void CPU::Handle_11_Opcodes(uint8_t instruction) {
    std::cout << "In Add";
}

std::map<uint8_t, CPU::functionPointer> CPU::OpCodeMethods = { 
    {0x00, &CPU::Handle_00_Opcodes},
    {0x40, &CPU::Handle_01_Opcodes},
    {0x80, &CPU::Handle_10_Opcodes},
    {0xC0, &CPU::Handle_11_Opcodes}
};
