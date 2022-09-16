#include <stdio.h>
#include "cpu.h"
#include "bootroms.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->PC = 0;
    this->SP = 0;
    this->debug = true;
    //this->loadBIOS(BootROMs::BIOS_DMG, 256, 0);

    //this->addressBus[0] = 0x93;
    //this->addressBus[1] = 0x8B;
    //this->registers[A] = 0x3e;
    //this->registers[F] = 0x00;
    //this->registers[B] = 0x01;
    //this->registers[C] = 0x01;
    //this->registers[D] = 0x01;
    //this->registers[E] = 0x40;
    //this->registers[H] = 0x01;
    //this->registers[L] = 0x01;
    //this->run();

    //this->step();
}

void CPU::loadBIOS(const uint8_t* ROM, int size, uint16_t address) {
    for (uint16_t i = 0; i < size; i++) {
        addressBus[address + i] = ROM[i];
    }
}

// This function is temporary, step function should be called by the Machine class
void CPU::run() {
    int x = 0; // Temporary set up, this loop should be infinite
    while (x < 10) {
        step();
        x++;
    }
}

void CPU::step() {
    uint8_t instruction = this->addressBus[this->PC];
    execute(instruction);
    if (debug) { this->printRegs(); }
}

void CPU::execute(uint8_t instruction) {
    // uint8_t opCode = instruction & 0xC0; // look at two most significant bits
    (this->*InstructionMethods1[instruction])(instruction);
    this->PC += 1;
}

void CPU::LD_R_to_R(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;

    if (r1 == 0x06) {
        if (r2 == 0x06) {
            // HALT INSTRUCTION
        }
        else {
            if (debug) { printf("LD (HL) to %c\n", regNames[r2]); }
            this->registers[r2] = this->addressBus[this->getHL()];
        }
    }
    else if (r2 == 0x06) {
        if (debug) { printf("LD %c to (HL)\n", regNames[r1]); }
        this->addressBus[this->getHL()] = this->registers[r1];
    }
    else {
        if (debug) { printf("LD %c to %c\n", regNames[r1], regNames[r2]); }
        this->registers[r2] = this->registers[r1];
    }
}

void CPU::LD_8_Bit(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;

    if (instruction > 0xC0) {
        if (r1 == 0x00) {
            uint16_t address;
            switch (r2) {
            case 0x04:
                address = 0xFF00 + this->addressBus[++(this->PC)];
                this->addressBus[address] = this->registers[A];
                if (debug) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + this->addressBus[++(this->PC)];
                this->registers[A] = this->addressBus[address];
                if (debug) { printf("LD A, (0x%04X)\n", address); }
                break;
            default:
                break;
            }
        }
        else if(r1 == 0x02) {
            uint16_t address;
            switch (r2) {
            case 0x04:
                address = 0xFF00 + this->registers[C];
                this->addressBus[address] = this->registers[A];
                if (debug) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x05:
                address = this->addressBus[++(this->PC)] + (this->addressBus[++(this->PC)] << 8);
                this->addressBus[address] = this->registers[A];
                if (debug) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + this->registers[C];
                this->registers[A] = this->addressBus[address];
                if (debug) { printf("LD A, (0x%04X)\n", address); }
                break;
            case 0x07:
                address = this->addressBus[++(this->PC)] + (this->addressBus[++(this->PC)] << 8);
                this->registers[A] = this->addressBus[address];
                if (debug) { printf("LD A, (0x%04X)\n", address); }
                break;
            default:
                break;
            }
        }
    }
    else {
        if (r1 == 0x02) {
            uint16_t hl = this->getHL();
            switch (r2)
            {
            case 0x00:
                this->addressBus[this->getBC()] = this->registers[A];
                if (debug) { printf("LD (BC), A\n"); }
                break;
            case 0x01:
                this->registers[A] = this->addressBus[this->getBC()];
                if (debug) { printf("LD A, (BC)\n"); }
                break;
            case 0x02:
                this->addressBus[this->getDE()] = this->registers[A];
                if (debug) { printf("LD (DE), A\n"); }
                break;
            case 0x03:
                this->registers[A] = this->addressBus[this->getDE()];
                if (debug) { printf("LD A, (DE)\n"); }
                break;
            case 0x04:
                this->addressBus[hl] = this->registers[A];
                this->setHL(hl + 1);
                if (debug) { printf("LD (HL+), A\n"); }
                break;
            case 0x05:
                this->registers[A] = this->addressBus[hl];
                this->setHL(hl + 1);
                if (debug) { printf("LD A, (HL+)\n"); }
                break;
            case 0x06:
                this->addressBus[hl] = this->registers[A];
                this->setHL(hl - 1);
                if (debug) { printf("LD (HL-), A\n"); }
                break;
            case 0x07:
                this->registers[A] = this->addressBus[hl];
                this->setHL(hl - 1);
                if (debug) { printf("LD A, (HL-)\n"); }
                break;
            default:
                break;
            }
        }
        else if (r1 == 0x06) {
            uint8_t val = this->addressBus[++(this->PC)];
            if (r2 == 0x06) {
                this->addressBus[this->getHL()] = val;
                if (debug) { printf("LD (HL), 0x%02X\n", val); }
            }
            else {
                this->registers[r2] = val;
                if (debug) { printf("LD %c, 0x%02X\n", regNames[r2], val); }
            }
        }
    }
}

void CPU::LD_16_Bit(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;

    if (instruction > 0xC0) {
        if (r1 == 0x01) {
            uint8_t lVal;
            uint8_t hVal;
            switch (r2)
            {
            case 0x00:
                lVal = this->addressBus[this->SP++];
                hVal = this->addressBus[this->SP++];
                this->setBC(hVal, lVal);
                if (debug) { printf("POP BC\n"); }
                break;
            case 0x02:
                lVal = this->addressBus[this->SP++];
                hVal = this->addressBus[this->SP++];
                this->setDE(hVal, lVal);
                if (debug) { printf("POP DE\n"); }
                break;
            case 0x04:
                lVal = this->addressBus[this->SP++];
                hVal = this->addressBus[this->SP++];
                this->setHL(hVal, lVal);
                if (debug) { printf("POP HL\n"); }
                break;
            case 0x06:
                lVal = this->addressBus[this->SP++];
                hVal = this->addressBus[this->SP++];
                this->setAF(hVal, lVal);
                if (debug) { printf("POP AF\n"); }
                break;
            case 0x07:
                this->SP = this->getHL();
                if (debug) { printf("LD SP, HL\n"); }
                break;
            default:
                break;
            }
        }
        else {
            int8_t val;
            uint16_t result;
            switch (r2)
            {
            case 0x00:
                this->addressBus[--this->SP] = this->registers[B];
                this->addressBus[--this->SP] = this->registers[C];
                if (debug) { printf("PUSH BC\n"); }
                break;
            case 0x02:
                this->addressBus[--this->SP] = this->registers[D];
                this->addressBus[--this->SP] = this->registers[E];
                if (debug) { printf("PUSH DE\n"); }
                break;
            case 0x04:
                this->addressBus[--this->SP] = this->registers[H];
                this->addressBus[--this->SP] = this->registers[L];
                if (debug) { printf("PUSH HL\n"); }
                break;
            case 0x06:
                this->addressBus[--this->SP] = this->registers[A];
                this->addressBus[--this->SP] = this->registers[F];
                if (debug) { printf("PUSH AF\n"); }
                break;
            case 0x07:
                val = this->addressBus[++(this->PC)];
                result = this->SP + val;
                this->setHL(result);

                // Calculate if Carry flag needs to be set
                (val > 0 && result < this->SP) ? this->setC(true) : this->setC(false);
                // Calculate if Half-Carry flag needs to be set
                (val > 0 && (result & 0x0FFF) < (this->SP & 0xFFF)) ? this->setH(true) : this->setH(false);
                // Set N and Z flags to 0
                this->setN(false);
                this->setZ(false);

                if (debug) { printf("LD SP, HL\n"); }
                break;
            default:
                break;
            }
        }
    }
    else {
        uint8_t lVal = this->addressBus[++(this->PC)];
        uint8_t hVal = this->addressBus[++(this->PC)];
        uint16_t addr;
        switch (r2)
        {
        case 0x00:
            this->setBC(hVal, lVal);
            if (debug) { printf("LD BC, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x01:
            addr = (hVal << 8) + lVal;
            this->addressBus[addr] = this->getSP() & 0x00FF;
            this->addressBus[addr + 1] = (this->getSP() & 0xFF00) >> 8;
            if (debug) { printf("LD (0x%04X), SP\n", addr); }
            break;
        case 0x02:
            this->setDE(hVal, lVal);
            if (debug) { printf("LD DE, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x04:
            this->setHL(hVal, lVal);
            if (debug) { printf("LD HL, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x06:
            this->SP = (hVal << 8) + lVal;
            if (debug) { printf("LD SP, 0x%02X%02X\n", hVal, lVal); }
            break;
        default:
            break;
        }
    }
}

void CPU::JP(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;


    if (r1 == 0x01) {
        ;
    }
    else if (r1 == 0x02) {
        ;
    }


}

void CPU::ADD(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (encoding == 0x03) {
        nVal = this->addressBus[++(this->PC)];
        if (debug) { printf("ADD A, 0x%02X\n", nVal); }
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            nVal = this->addressBus[this->getHL()];
            if (debug) { printf("ADD A, (HL)\n"); }
        }
        else {
            nVal = this->registers[r];
            if (debug) { printf("ADD A, %c\n", this->regNames[r]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    (((nVal & 0x0F) + (rVal & 0x0F)) > 0x0F) ? this->setH(true) : this->setH(false);
    // Perform addition to A register
    this->registers[A] += nVal;
    // Calculate if Full-Carry flag needs to be set
    (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);

    this->setN(false);
}

void CPU::ADC(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (encoding == 0x03) {
        nVal = this->addressBus[++(this->PC)];
        if (debug) { printf("ADC A, 0x%02X\n", nVal); }
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            nVal = this->addressBus[this->getHL()];
            if (debug) { printf("ADC A, (HL)\n"); }
        }
        else {
            nVal = this->registers[r];
            if (debug) { printf("ADC A, %c\n", this->regNames[r]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    (((nVal & 0x0F) + (rVal & 0x0F) + this->getC()) > 0x0F) ? this->setH(true) : this->setH(false);
    // Perform addition to A register
    this->registers[A] += nVal + this->getC();
    // Calculate if Full-Carry flag needs to be set
    (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);

    this->setN(false);
}

void CPU::SUB(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (encoding == 0x03) {
        nVal = this->addressBus[++(this->PC)];
        if (debug) { printf("SUB A, 0x%02X\n", nVal); }
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            nVal = this->addressBus[this->getHL()];
            if (debug) { printf("SUB A, (HL)\n"); }
        }
        else {
            nVal = this->registers[r];
            if (debug) { printf("SUB A, %c\n", this->regNames[r]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    ((nVal & 0x0F) > (rVal & 0x0F)) ? this->setH(true) : this->setH(false);
    // Perform subtraction to A register
    this->registers[A] -= nVal;
    // Calculate if Full-Carry flag needs to be set
    (nVal > rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);

    this->setN(true);
}

void CPU::SBC(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (encoding == 0x03) {
        nVal = this->addressBus[++(this->PC)];
        if (debug) { printf("SBC A, 0x%02X\n", nVal); }
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            nVal = this->addressBus[this->getHL()];
            if (debug) { printf("SBC A, (HL)\n"); }
        }
        else {
            nVal = this->registers[r];
            if (debug) { printf("SBC A, %c\n", this->regNames[r]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    ((rVal & 0x0F) < (nVal & 0x0F) + this->getC()) ? this->setH(true) : this->setH(false);
    // I don't understand, below line *should* be the right way to calc h-bit, but programming manual
    // and java translate source says otherwise without much explanation
    // uint16_t operand = nVal + this->getC();
    // ((operand <= 0xFF) && ((operand & 0x000F) > (rVal & 0x0F))) ? this->setH(true) : this->setH(false);
    // Perform subtraction to A register
    this->registers[A] -= nVal + this->getC();
    // Calculate if Full-Carry flag needs to be set
    (nVal + this->getC() > rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);

    this->setN(true);
}

void CPU::AND(uint8_t instruction) {
    std::cout << "AND A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];
        this->registers[A] &= nVal;
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            this->registers[A] &= this->addressBus[this->getHL()];
        }
        else {
            this->registers[A] &= this->registers[r];
        }
    }

    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    // Set C and N flags to 0, H flag to 1
    this->setC(false);
    this->setH(true);
    this->setN(false);
}

void CPU::XOR(uint8_t instruction) {
    std::cout << "XOR A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];
        this->registers[A] ^= nVal;
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            this->registers[A] ^= this->addressBus[this->getHL()];
        }
        else {
            this->registers[A] ^= this->registers[r];
        }
    }

    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    // Set C, H, and N flags to 0
    this->setC(false);
    this->setH(false);
    this->setN(false);
}

void CPU::OR(uint8_t instruction) {
    std::cout << "OR A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];
        this->registers[A] |= nVal;
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            this->registers[A] |= this->addressBus[this->getHL()];
        }
        else {
            this->registers[A] |= this->registers[r];
        }
    }

    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    // Set C, H, and N flags to 0
    this->setC(false);
    this->setH(false);
    this->setN(false);
}

void CPU::CP(uint8_t instruction) {
    std::cout << "CP A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (encoding == 0x03) {
        nVal = this->addressBus[++(this->PC)];
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            nVal = this->addressBus[this->getHL()];
        }
        else {
            nVal = this->registers[r];
        }
    }

    // Calculate if Half-Carry flag needs to be set
    (nVal < rVal) ? this->setH(true) : this->setH(false);
    // Calculate if Full-Carry flag needs to be set
    (nVal > rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (nVal == rVal) ? this->setZ(true) : this->setZ(false);

    this->setN(true);
}

void CPU::CBPrefix(uint8_t instruction) {
    if (debug) { printf("CB Prefix\n"); }
    instruction = this->addressBus[++(this->PC)];
    (this->*InstructionMethods2[instruction])(instruction);
    this->PC += 1;
}

void CPU::RLC(uint8_t instruction) {
    ;
}

void CPU::RRC(uint8_t instruction) {
    ;
}

void CPU::RL(uint8_t instruction) {
    ;
}

void CPU::RR(uint8_t instruction) {
    ;
}

void CPU::SLA(uint8_t instruction) {
    ;
}

void CPU::SRA(uint8_t instruction) {
    ;
}

void CPU::SWAP(uint8_t instruction) {
    ;
}

void CPU::SRL(uint8_t instruction) {
    ;
}

void CPU::BIT(uint8_t instruction) {
    uint8_t r = instruction & 0x07;
    uint8_t n = (instruction & 0x38) >> 3;

    uint8_t mask = 0x01 << n;

    if (r == 0x06) {
        if (debug) { printf("BIT test %d in (HL)\n", n); }
        // Calculate if Zero flag needs to be set
        ((this->addressBus[this->getHL()] & mask) == 0) ? this->setZ(true) : this->setZ(false);
    }
    else {
        if (debug) { printf("BIT test %d in %c\n", n, regNames[r]); }
        // Calculate if Zero flag needs to be set
        ((this->registers[r] & mask) == 0) ? this->setZ(true) : this->setZ(false);
    }

    // Set H flag to 1, N flag to 0
    this->setH(true);
    this->setN(false);
}

void CPU::RES(uint8_t instruction) {
    uint8_t r = instruction & 0x07;
    uint8_t n = (instruction & 0x38) >> 3;

    uint8_t mask = (0x01 << n) ^ 0xFF;

    if (r == 0x06) {
        if (debug) { printf("RES bit %d in (HL)\n", n); }
        this->addressBus[this->getHL()] &= mask;
    }
    else {
        if (debug) { printf("RES bit %d in %c\n", n, regNames[r]); }
        this->registers[r] &= mask;
    }
}

void CPU::SET(uint8_t instruction) {
    uint8_t r = instruction & 0x07;
    uint8_t n = (instruction & 0x38) >> 3;

    uint8_t mask = 0x01 << n;

    if (r == 0x06) {
        if (debug) { printf("SET bit %d in (HL)\n", n); }
        this->addressBus[this->getHL()] |= mask;
    }
    else {
        if (debug) { printf("SET bit %d in %c\n", n, regNames[r]); }
        this->registers[r] |= mask;
    }
}

void CPU::printRegs() {
    printf("REGS: \nA: 0x%02X F: 0x%02X\nB: 0x%02X C: 0x%02X\nD: 0x%02X E: 0x%02X\nH: 0x%02X L: 0x%02X\nPC: 0x%04X\nSP: 0x%04X\n\n", 
        this->registers[A], this->registers[F], this->registers[B], this->registers[C],
        this->registers[D], this->registers[E], this->registers[H], this->registers[L],
        this->PC, this->SP);
}

void CPU::nop(uint8_t instruction) {
    std::cout << std::hex << int(instruction) << " nop" << std::endl;
}

uint16_t CPU::getAF() {
    return (this->registers[A] << 8) + this->registers[F];
}

uint16_t CPU::getBC() {
    return (this->registers[B] << 8) + this->registers[C];
}

uint16_t CPU::getDE() {
    return (this->registers[D] << 8) + this->registers[E];
}

uint16_t CPU::getHL() {
    return (this->registers[H] << 8) + this->registers[L];
}

uint16_t CPU::getSP() {
    return this->SP;
}

void CPU::setAF(uint8_t hVal, uint8_t lVal) {
    this->registers[A] = hVal;
    this->registers[F] = lVal;
}

void CPU::setBC(uint8_t hVal, uint8_t lVal) {
    this->registers[B] = hVal;
    this->registers[C] = lVal;
}

void CPU::setDE(uint8_t hVal, uint8_t lVal) {
    this->registers[D] = hVal;
    this->registers[E] = lVal;
}

void CPU::setHL(uint8_t hVal, uint8_t lVal) {
    this->registers[H] = hVal;
    this->registers[L] = lVal;
}

void CPU::setAF(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers[A] = hVal;
    this->registers[F] = lVal;
}

void CPU::setBC(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers[B] = hVal;
    this->registers[C] = lVal;
}

void CPU::setDE(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers[D] = hVal;
    this->registers[E] = lVal;
}

void CPU::setHL(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers[H] = hVal;
    this->registers[L] = lVal;
}

bool CPU::getC() {
    return this->registers[F] & 0b00010000;
}

bool CPU::getH() {
    return this->registers[F] & 0b00100000;
}

bool CPU::getN() {
    return this->registers[F] & 0b01000000;
}

bool CPU::getZ() {
    return this->registers[F] & 0b10000000;
}

void CPU::setC(bool val) {
    if (val == true) {
        this->registers[F] |= 0b00010000;
    }
    else {
        this->registers[F] &= 0b11101111;
    }
}

void CPU::setH(bool val) {
    if (val == true) {
        this->registers[F] |= 0b00100000;
    }
    else {
        this->registers[F] &= 0b11011111;
    }
}

void CPU::setN(bool val) {
    if (val == true) {
        this->registers[F] |= 0b01000000;
    }
    else {
        this->registers[F] &= 0b10111111;
    }
}

void CPU::setZ(bool val) {
    if (val == true) {
        this->registers[F] |= 0b10000000;
    }
    else {
        this->registers[F] &= 0b01111111;
    }
}

std::map<uint8_t, CPU::functionPointer> CPU::InstructionMethods1 = { 
    {0X00, &CPU::nop},
    {0X01, &CPU::LD_16_Bit},
    {0X02, &CPU::LD_8_Bit},
    {0X03, &CPU::nop},
    {0X04, &CPU::nop},
    {0X05, &CPU::nop},
    {0X06, &CPU::LD_8_Bit},
    {0X07, &CPU::nop},
    {0X08, &CPU::LD_16_Bit},
    {0X09, &CPU::nop},
    {0X0A, &CPU::LD_8_Bit},
    {0X0B, &CPU::nop},
    {0X0C, &CPU::nop},
    {0X0D, &CPU::nop},
    {0X0E, &CPU::LD_8_Bit},
    {0X0F, &CPU::nop},
    {0X10, &CPU::nop},
    {0X11, &CPU::LD_16_Bit},
    {0X12, &CPU::LD_8_Bit},
    {0X13, &CPU::nop},
    {0X14, &CPU::nop},
    {0X15, &CPU::nop},
    {0X16, &CPU::LD_8_Bit},
    {0X17, &CPU::nop},
    {0X18, &CPU::nop},
    {0X19, &CPU::nop},
    {0X1A, &CPU::LD_8_Bit},
    {0X1B, &CPU::nop},
    {0X1C, &CPU::nop},
    {0X1D, &CPU::nop},
    {0X1E, &CPU::LD_8_Bit},
    {0X1F, &CPU::nop},
    {0X20, &CPU::nop},
    {0X21, &CPU::LD_16_Bit},
    {0X22, &CPU::LD_8_Bit},
    {0X23, &CPU::nop},
    {0X24, &CPU::nop},
    {0X25, &CPU::nop},
    {0X26, &CPU::LD_8_Bit},
    {0X27, &CPU::nop},
    {0X28, &CPU::nop},
    {0X29, &CPU::nop},
    {0X2A, &CPU::LD_8_Bit},
    {0X2B, &CPU::nop},
    {0X2C, &CPU::nop},
    {0X2D, &CPU::nop},
    {0X2E, &CPU::LD_8_Bit},
    {0X2F, &CPU::nop},
    {0X30, &CPU::nop},
    {0X31, &CPU::LD_16_Bit},
    {0X32, &CPU::LD_8_Bit},
    {0X33, &CPU::nop},
    {0X34, &CPU::nop},
    {0X35, &CPU::nop},
    {0X36, &CPU::LD_8_Bit},
    {0X37, &CPU::nop},
    {0X38, &CPU::nop},
    {0X39, &CPU::nop},
    {0X3A, &CPU::LD_8_Bit},
    {0X3B, &CPU::nop},
    {0X3C, &CPU::nop},
    {0X3D, &CPU::nop},
    {0X3E, &CPU::LD_8_Bit},
    {0X3F, &CPU::nop},
    {0X40, &CPU::LD_R_to_R},
    {0X41, &CPU::LD_R_to_R},
    {0X42, &CPU::LD_R_to_R},
    {0X43, &CPU::LD_R_to_R},
    {0X44, &CPU::LD_R_to_R},
    {0X45, &CPU::LD_R_to_R},
    {0X46, &CPU::LD_R_to_R},
    {0X47, &CPU::LD_R_to_R},
    {0X48, &CPU::LD_R_to_R},
    {0X49, &CPU::LD_R_to_R},
    {0X4A, &CPU::LD_R_to_R},
    {0X4B, &CPU::LD_R_to_R},
    {0X4C, &CPU::LD_R_to_R},
    {0X4D, &CPU::LD_R_to_R},
    {0X4E, &CPU::LD_R_to_R},
    {0X4F, &CPU::LD_R_to_R},
    {0X50, &CPU::LD_R_to_R},
    {0X51, &CPU::LD_R_to_R},
    {0X52, &CPU::LD_R_to_R},
    {0X53, &CPU::LD_R_to_R},
    {0X54, &CPU::LD_R_to_R},
    {0X55, &CPU::LD_R_to_R},
    {0X56, &CPU::LD_R_to_R},
    {0X57, &CPU::LD_R_to_R},
    {0X58, &CPU::LD_R_to_R},
    {0X59, &CPU::LD_R_to_R},
    {0X5A, &CPU::LD_R_to_R},
    {0X5B, &CPU::LD_R_to_R},
    {0X5C, &CPU::LD_R_to_R},
    {0X5D, &CPU::LD_R_to_R},
    {0X5E, &CPU::LD_R_to_R},
    {0X5F, &CPU::LD_R_to_R},
    {0X60, &CPU::LD_R_to_R},
    {0X61, &CPU::LD_R_to_R},
    {0X62, &CPU::LD_R_to_R},
    {0X63, &CPU::LD_R_to_R},
    {0X64, &CPU::LD_R_to_R},
    {0X65, &CPU::LD_R_to_R},
    {0X66, &CPU::LD_R_to_R},
    {0X67, &CPU::LD_R_to_R},
    {0X68, &CPU::LD_R_to_R},
    {0X69, &CPU::LD_R_to_R},
    {0X6A, &CPU::LD_R_to_R},
    {0X6B, &CPU::LD_R_to_R},
    {0X6C, &CPU::LD_R_to_R},
    {0X6D, &CPU::LD_R_to_R},
    {0X6E, &CPU::LD_R_to_R},
    {0X6F, &CPU::LD_R_to_R},
    {0X70, &CPU::LD_R_to_R},
    {0X71, &CPU::LD_R_to_R},
    {0X72, &CPU::LD_R_to_R},
    {0X73, &CPU::LD_R_to_R},
    {0X74, &CPU::LD_R_to_R},
    {0X75, &CPU::LD_R_to_R},
    {0X76, &CPU::LD_R_to_R},
    {0X77, &CPU::LD_R_to_R},
    {0X78, &CPU::LD_R_to_R},
    {0X79, &CPU::LD_R_to_R},
    {0X7A, &CPU::LD_R_to_R},
    {0X7B, &CPU::LD_R_to_R},
    {0X7C, &CPU::LD_R_to_R},
    {0X7D, &CPU::LD_R_to_R},
    {0X7E, &CPU::LD_R_to_R},
    {0X7F, &CPU::LD_R_to_R},
    {0X80, &CPU::ADD},
    {0X81, &CPU::ADD},
    {0X82, &CPU::ADD},
    {0X83, &CPU::ADD},
    {0X84, &CPU::ADD},
    {0X85, &CPU::ADD},
    {0X86, &CPU::ADD},
    {0X87, &CPU::ADD},
    {0X88, &CPU::ADC},
    {0X89, &CPU::ADC},
    {0X8A, &CPU::ADC},
    {0X8B, &CPU::ADC},
    {0X8C, &CPU::ADC},
    {0X8D, &CPU::ADC},
    {0X8E, &CPU::ADC},
    {0X8F, &CPU::ADC},
    {0X90, &CPU::SUB},
    {0X91, &CPU::SUB},
    {0X92, &CPU::SUB},
    {0X93, &CPU::SUB},
    {0X94, &CPU::SUB},
    {0X95, &CPU::SUB},
    {0X96, &CPU::SUB},
    {0X97, &CPU::SUB},
    {0X98, &CPU::SBC},
    {0X99, &CPU::SBC},
    {0X9A, &CPU::SBC},
    {0X9B, &CPU::SBC},
    {0X9C, &CPU::SBC},
    {0X9D, &CPU::SBC},
    {0X9E, &CPU::SBC},
    {0X9F, &CPU::SBC},
    {0XA0, &CPU::AND},
    {0XA1, &CPU::AND},
    {0XA2, &CPU::AND},
    {0XA3, &CPU::AND},
    {0XA4, &CPU::AND},
    {0XA5, &CPU::AND},
    {0XA6, &CPU::AND},
    {0XA7, &CPU::AND},
    {0XA8, &CPU::XOR},
    {0XA9, &CPU::XOR},
    {0XAA, &CPU::XOR},
    {0XAB, &CPU::XOR},
    {0XAC, &CPU::XOR},
    {0XAD, &CPU::XOR},
    {0XAE, &CPU::XOR},
    {0XAF, &CPU::XOR},
    {0XB0, &CPU::OR},
    {0XB1, &CPU::OR},
    {0XB2, &CPU::OR},
    {0XB3, &CPU::OR},
    {0XB4, &CPU::OR},
    {0XB5, &CPU::OR},
    {0XB6, &CPU::OR},
    {0XB7, &CPU::OR},
    {0XB8, &CPU::CP},
    {0XB9, &CPU::CP},
    {0XBA, &CPU::CP},
    {0XBB, &CPU::CP},
    {0XBC, &CPU::CP},
    {0XBD, &CPU::CP},
    {0XBE, &CPU::CP},
    {0XBF, &CPU::CP},
    {0XC0, &CPU::nop},
    {0XC1, &CPU::LD_16_Bit},
    {0XC2, &CPU::nop},
    {0XC3, &CPU::nop},
    {0XC4, &CPU::nop},
    {0XC5, &CPU::LD_16_Bit},
    {0XC6, &CPU::ADD},
    {0XC7, &CPU::nop},
    {0XC8, &CPU::nop},
    {0XC9, &CPU::nop},
    {0XCA, &CPU::nop},
    {0XCB, &CPU::CBPrefix},
    {0XCC, &CPU::nop},
    {0XCD, &CPU::nop},
    {0XCE, &CPU::ADC},
    {0XCF, &CPU::nop},
    {0XD0, &CPU::nop},
    {0XD1, &CPU::LD_16_Bit},
    {0XD2, &CPU::nop},
    {0XD3, &CPU::nop},
    {0XD4, &CPU::nop},
    {0XD5, &CPU::LD_16_Bit},
    {0XD6, &CPU::SUB},
    {0XD7, &CPU::nop},
    {0XD8, &CPU::nop},
    {0XD9, &CPU::nop},
    {0XDA, &CPU::nop},
    {0XDB, &CPU::nop},
    {0XDC, &CPU::nop},
    {0XDD, &CPU::nop},
    {0XDE, &CPU::SBC},
    {0XDF, &CPU::nop},
    {0XE0, &CPU::LD_8_Bit},
    {0XE1, &CPU::LD_16_Bit},
    {0XE2, &CPU::LD_8_Bit},
    {0XE3, &CPU::nop},
    {0XE4, &CPU::nop},
    {0XE5, &CPU::LD_16_Bit},
    {0XE6, &CPU::AND},
    {0XE7, &CPU::nop},
    {0XE8, &CPU::nop},
    {0XE9, &CPU::nop},
    {0XEA, &CPU::LD_8_Bit},
    {0XEB, &CPU::nop},
    {0XEC, &CPU::nop},
    {0XED, &CPU::nop},
    {0XEE, &CPU::XOR},
    {0XEF, &CPU::nop},
    {0XF0, &CPU::LD_8_Bit},
    {0XF1, &CPU::LD_16_Bit},
    {0XF2, &CPU::LD_8_Bit},
    {0XF3, &CPU::nop},
    {0XF4, &CPU::nop},
    {0XF5, &CPU::LD_16_Bit},
    {0XF6, &CPU::OR},
    {0XF7, &CPU::nop},
    {0XF8, &CPU::LD_16_Bit},
    {0XF9, &CPU::LD_16_Bit},
    {0XFA, &CPU::LD_8_Bit},
    {0XFB, &CPU::nop},
    {0XFC, &CPU::nop},
    {0XFD, &CPU::nop},
    {0XFE, &CPU::CP},
    {0XFF, &CPU::nop},
};

std::map<uint8_t, CPU::functionPointer> CPU::InstructionMethods2 = {
    {0X00, &CPU::nop},
    {0X01, &CPU::nop},
    {0X02, &CPU::nop},
    {0X03, &CPU::nop},
    {0X04, &CPU::nop},
    {0X05, &CPU::nop},
    {0X06, &CPU::nop},
    {0X07, &CPU::nop},
    {0X08, &CPU::nop},
    {0X09, &CPU::nop},
    {0X0A, &CPU::nop},
    {0X0B, &CPU::nop},
    {0X0C, &CPU::nop},
    {0X0D, &CPU::nop},
    {0X0E, &CPU::nop},
    {0X0F, &CPU::nop},
    {0X10, &CPU::nop},
    {0X11, &CPU::nop},
    {0X12, &CPU::nop},
    {0X13, &CPU::nop},
    {0X14, &CPU::nop},
    {0X15, &CPU::nop},
    {0X16, &CPU::nop},
    {0X17, &CPU::nop},
    {0X18, &CPU::nop},
    {0X19, &CPU::nop},
    {0X1A, &CPU::nop},
    {0X1B, &CPU::nop},
    {0X1C, &CPU::nop},
    {0X1D, &CPU::nop},
    {0X1E, &CPU::nop},
    {0X1F, &CPU::nop},
    {0X20, &CPU::nop},
    {0X21, &CPU::nop},
    {0X22, &CPU::nop},
    {0X23, &CPU::nop},
    {0X24, &CPU::nop},
    {0X25, &CPU::nop},
    {0X26, &CPU::nop},
    {0X27, &CPU::nop},
    {0X28, &CPU::nop},
    {0X29, &CPU::nop},
    {0X2A, &CPU::nop},
    {0X2B, &CPU::nop},
    {0X2C, &CPU::nop},
    {0X2D, &CPU::nop},
    {0X2E, &CPU::nop},
    {0X2F, &CPU::nop},
    {0X30, &CPU::nop},
    {0X31, &CPU::nop},
    {0X32, &CPU::nop},
    {0X33, &CPU::nop},
    {0X34, &CPU::nop},
    {0X35, &CPU::nop},
    {0X36, &CPU::nop},
    {0X37, &CPU::nop},
    {0X38, &CPU::nop},
    {0X39, &CPU::nop},
    {0X3A, &CPU::nop},
    {0X3B, &CPU::nop},
    {0X3C, &CPU::nop},
    {0X3D, &CPU::nop},
    {0X3E, &CPU::nop},
    {0X3F, &CPU::nop},
    {0X40, &CPU::BIT},
    {0X41, &CPU::BIT},
    {0X42, &CPU::BIT},
    {0X43, &CPU::BIT},
    {0X44, &CPU::BIT},
    {0X45, &CPU::BIT},
    {0X46, &CPU::BIT},
    {0X47, &CPU::BIT},
    {0X48, &CPU::BIT},
    {0X49, &CPU::BIT},
    {0X4A, &CPU::BIT},
    {0X4B, &CPU::BIT},
    {0X4C, &CPU::BIT},
    {0X4D, &CPU::BIT},
    {0X4E, &CPU::BIT},
    {0X4F, &CPU::BIT},
    {0X50, &CPU::BIT},
    {0X51, &CPU::BIT},
    {0X52, &CPU::BIT},
    {0X53, &CPU::BIT},
    {0X54, &CPU::BIT},
    {0X55, &CPU::BIT},
    {0X56, &CPU::BIT},
    {0X57, &CPU::BIT},
    {0X58, &CPU::BIT},
    {0X59, &CPU::BIT},
    {0X5A, &CPU::BIT},
    {0X5B, &CPU::BIT},
    {0X5C, &CPU::BIT},
    {0X5D, &CPU::BIT},
    {0X5E, &CPU::BIT},
    {0X5F, &CPU::BIT},
    {0X60, &CPU::BIT},
    {0X61, &CPU::BIT},
    {0X62, &CPU::BIT},
    {0X63, &CPU::BIT},
    {0X64, &CPU::BIT},
    {0X65, &CPU::BIT},
    {0X66, &CPU::BIT},
    {0X67, &CPU::BIT},
    {0X68, &CPU::BIT},
    {0X69, &CPU::BIT},
    {0X6A, &CPU::BIT},
    {0X6B, &CPU::BIT},
    {0X6C, &CPU::BIT},
    {0X6D, &CPU::BIT},
    {0X6E, &CPU::BIT},
    {0X6F, &CPU::BIT},
    {0X70, &CPU::BIT},
    {0X71, &CPU::BIT},
    {0X72, &CPU::BIT},
    {0X73, &CPU::BIT},
    {0X74, &CPU::BIT},
    {0X75, &CPU::BIT},
    {0X76, &CPU::BIT},
    {0X77, &CPU::BIT},
    {0X78, &CPU::BIT},
    {0X79, &CPU::BIT},
    {0X7A, &CPU::BIT},
    {0X7B, &CPU::BIT},
    {0X7C, &CPU::BIT},
    {0X7D, &CPU::BIT},
    {0X7E, &CPU::BIT},
    {0X7F, &CPU::BIT},
    {0X80, &CPU::RES},
    {0X81, &CPU::RES},
    {0X82, &CPU::RES},
    {0X83, &CPU::RES},
    {0X84, &CPU::RES},
    {0X85, &CPU::RES},
    {0X86, &CPU::RES},
    {0X87, &CPU::RES},
    {0X88, &CPU::RES},
    {0X89, &CPU::RES},
    {0X8A, &CPU::RES},
    {0X8B, &CPU::RES},
    {0X8C, &CPU::RES},
    {0X8D, &CPU::RES},
    {0X8E, &CPU::RES},
    {0X8F, &CPU::RES},
    {0X90, &CPU::RES},
    {0X91, &CPU::RES},
    {0X92, &CPU::RES},
    {0X93, &CPU::RES},
    {0X94, &CPU::RES},
    {0X95, &CPU::RES},
    {0X96, &CPU::RES},
    {0X97, &CPU::RES},
    {0X98, &CPU::RES},
    {0X99, &CPU::RES},
    {0X9A, &CPU::RES},
    {0X9B, &CPU::RES},
    {0X9C, &CPU::RES},
    {0X9D, &CPU::RES},
    {0X9E, &CPU::RES},
    {0X9F, &CPU::RES},
    {0XA0, &CPU::RES},
    {0XA1, &CPU::RES},
    {0XA2, &CPU::RES},
    {0XA3, &CPU::RES},
    {0XA4, &CPU::RES},
    {0XA5, &CPU::RES},
    {0XA6, &CPU::RES},
    {0XA7, &CPU::RES},
    {0XA8, &CPU::RES},
    {0XA9, &CPU::RES},
    {0XAA, &CPU::RES},
    {0XAB, &CPU::RES},
    {0XAC, &CPU::RES},
    {0XAD, &CPU::RES},
    {0XAE, &CPU::RES},
    {0XAF, &CPU::RES},
    {0XB0, &CPU::RES},
    {0XB1, &CPU::RES},
    {0XB2, &CPU::RES},
    {0XB3, &CPU::RES},
    {0XB4, &CPU::RES},
    {0XB5, &CPU::RES},
    {0XB6, &CPU::RES},
    {0XB7, &CPU::RES},
    {0XB8, &CPU::RES},
    {0XB9, &CPU::RES},
    {0XBA, &CPU::RES},
    {0XBB, &CPU::RES},
    {0XBC, &CPU::RES},
    {0XBD, &CPU::RES},
    {0XBE, &CPU::RES},
    {0XBF, &CPU::RES},
    {0XC0, &CPU::SET},
    {0XC1, &CPU::SET},
    {0XC2, &CPU::SET},
    {0XC3, &CPU::SET},
    {0XC4, &CPU::SET},
    {0XC5, &CPU::SET},
    {0XC6, &CPU::SET},
    {0XC7, &CPU::SET},
    {0XC8, &CPU::SET},
    {0XC9, &CPU::SET},
    {0XCA, &CPU::SET},
    {0XCB, &CPU::SET},
    {0XCC, &CPU::SET},
    {0XCD, &CPU::SET},
    {0XCE, &CPU::SET},
    {0XCF, &CPU::SET},
    {0XD0, &CPU::SET},
    {0XD1, &CPU::SET},
    {0XD2, &CPU::SET},
    {0XD3, &CPU::SET},
    {0XD4, &CPU::SET},
    {0XD5, &CPU::SET},
    {0XD6, &CPU::SET},
    {0XD7, &CPU::SET},
    {0XD8, &CPU::SET},
    {0XD9, &CPU::SET},
    {0XDA, &CPU::SET},
    {0XDB, &CPU::SET},
    {0XDC, &CPU::SET},
    {0XDD, &CPU::SET},
    {0XDE, &CPU::SET},
    {0XDF, &CPU::SET},
    {0XE0, &CPU::SET},
    {0XE1, &CPU::SET},
    {0XE2, &CPU::SET},
    {0XE3, &CPU::SET},
    {0XE4, &CPU::SET},
    {0XE5, &CPU::SET},
    {0XE6, &CPU::SET},
    {0XE7, &CPU::SET},
    {0XE8, &CPU::SET},
    {0XE9, &CPU::SET},
    {0XEA, &CPU::SET},
    {0XEB, &CPU::SET},
    {0XEC, &CPU::SET},
    {0XED, &CPU::SET},
    {0XEE, &CPU::SET},
    {0XEF, &CPU::SET},
    {0XF0, &CPU::SET},
    {0XF1, &CPU::SET},
    {0XF2, &CPU::SET},
    {0XF3, &CPU::SET},
    {0XF4, &CPU::SET},
    {0XF5, &CPU::SET},
    {0XF6, &CPU::SET},
    {0XF7, &CPU::SET},
    {0XF8, &CPU::SET},
    {0XF9, &CPU::SET},
    {0XFA, &CPU::SET},
    {0XFB, &CPU::SET},
    {0XFC, &CPU::SET},
    {0XFD, &CPU::SET},
    {0XFE, &CPU::SET},
    {0XFF, &CPU::SET},
};
