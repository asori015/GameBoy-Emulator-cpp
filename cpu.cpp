#include <stdio.h>
#include "cpu.h"
#include "bootroms.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->PC = 0;
    this->SP = 0;
    //this->loadBIOS(0);

    this->addressBus[0] = 0x93;
    // this->addressBus[1] = 0x8B;
    this->registers[A] = 0x3e;
    this->registers[F] = 0x00;
    this->registers[B] = 0x01;
    this->registers[C] = 0x01;
    this->registers[D] = 0x01;
    this->registers[E] = 0x40;
    this->registers[H] = 0x01;
    this->registers[L] = 0x01;
    this->run();

    //this->step();
}

void CPU::loadBIOS(uint16_t address) {
    const uint8_t* ROM = BootROMs::BIOS_DMG;
    for (uint16_t i = 0; i < 256; i++) {
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
    debug();
    //std::cout << "In Step";
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
            this->registers[r2] = this->addressBus[this->getHL()];
            //std::cout << "In Load: " << int(this->registers[r2]) << " -> " << int(this->registers->HL()) << std::endl;
        }
    }
    else if (r2 == 0x06) {
        this->addressBus[this->getHL()] = this->registers[r1];
        //std::cout << "In Load: " << int(this->registers[r2]) << " -> " << int(r2) << std::endl;
    }
    else {
        this->registers[r2] = this->registers[r1];
        //std::cout << "In Load: " << int(r1) << " -> " << int(r2) << std::endl;
    }
}

void CPU::LD_8_Bit(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;
    /*if (r1 == 0x01) {
        switch (r2)
        {
        case 0x00:
            this->AddressBus[this->registers->getBC()] = this->registers[A];
            break;
        case 0x01:
            this->registers[A] = this->AddressBus[this->registers->getBC()];
            break;
        case 0x02:
            this->AddressBus[this->registers->getDE()] = this->registers[A];
            break;
        case 0x03:
            this->registers[A] = this->AddressBus[this->registers->getDE()];
            break;
        case 0x04:
            this->AddressBus[hl] = this->registers[A];
            hl += 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x05:
            this->registers[A] = this->AddressBus[hl];
            hl += 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x06:
            this->AddressBus[hl] = this->registers[A];
            hl -= 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x07:
            this->registers[A] = this->AddressBus[hl];
            hl -= 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        default:
            break;
        }
    }*/
    if (r1 == 0x02) {
        uint16_t hl = this->getHL();
        switch (r2)
        {
        case 0x00:
            this->addressBus[this->getBC()] = this->registers[A];
            break;
        case 0x01:
            this->registers[A] = this->addressBus[this->getBC()];
            break;
        case 0x02:
            this->addressBus[this->getDE()] = this->registers[A];
            break;
        case 0x03:
            this->registers[A] = this->addressBus[this->getDE()];
            break;
        case 0x04:
            this->addressBus[hl] = this->registers[A];
            hl += 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x05:
            this->registers[A] = this->addressBus[hl];
            hl += 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x06:
            this->addressBus[hl] = this->registers[A];
            hl -= 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        case 0x07:
            this->registers[A] = this->addressBus[hl];
            hl -= 1;
            this->registers[H] = uint8_t(hl >> 8);
            this->registers[L] = uint8_t(hl & 0x00FF);
            break;
        default:
            break;
        }
    }
    else if (r1 == 0x06) {
        if (r2 == 0x06) {
            this->addressBus[this->getHL()] = this->addressBus[this->PC];
        }
        else {
            this->registers[r2] = this->addressBus[this->PC];
        }
    }
}

void CPU::LD_16_Bit(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b00110000) >> 4;
    uint8_t lVal = this->addressBus[++(this->PC)];
    uint8_t hVal = this->addressBus[++(this->PC)];
    switch (encoding)
    {
    case 0x00:
        this->setBC(hVal, lVal);
        break;
    case 0x01:
        this->setDE(hVal, lVal);
        break;
    case 0x02:
        this->setHL(hVal, lVal);
        break;
    case 0x03:
        this->SP = (hVal << 8) + lVal;
        //std::cout << "SP = " << std::hex << int(this->SP) << std::endl;
        break;
    default:
        break;
    }
}

void CPU::ADD(uint8_t instruction) {
    std::cout << "ADD A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];

        // Calculate if Half-Carry flag needs to be set
        (((nVal & 0x0F) + (rVal & 0x0F)) > 0x0F) ? this->setH(true) : this->setH(false);
        // Perform addition to A register
        this->registers[A] += nVal;
        // Calculate if Full-Carry flag needs to be set
        (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
        // Calculate if Zero flag needs to be set
        (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            uint8_t nVal = this->addressBus[this->getHL()];

            // Calculate if Half-Carry flag needs to be set
            (((nVal & 0x0F) + (rVal & 0x0F)) > 0x0F) ? this->setH(true) : this->setH(false);
            // Perform addition to A register
            this->registers[A] += nVal;
            // Calculate if Full-Carry flag needs to be set
            (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
            // Calculate if Zero flag needs to be set
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
        else {
            uint8_t nVal = this->registers[r];

            // Calculate if Half-Carry flag needs to be set
            (((nVal & 0x0F) + (rVal & 0x0F)) > 0x0F) ? this->setH(true) : this->setH(false);
            // Perform addition to A register
            this->registers[A] += nVal;
            // Calculate if Full-Carry flag needs to be set
            (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
            // Calculate if Zero flag needs to be set
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
    }
    this->setN(false);
}

void CPU::ADC(uint8_t instruction) {
    std::cout << "ADC A " << std::endl;

    uint8_t encoding = (instruction & 0b11000000) >> 6;
    uint8_t rVal = this->registers[A];
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];

        // Calculate if Half-Carry flag needs to be set
        (((nVal & 0x0F) + (rVal & 0x0F) + this->getC()) > 0x0F) ? this->setH(true) : this->setH(false);
        // Perform addition to A register
        this->registers[A] += nVal + this->getC();
        // Calculate if Full-Carry flag needs to be set
        (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
        // Calculate if Zero flag needs to be set
        (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            uint8_t nVal = this->addressBus[this->getHL()];

            // Calculate if Half-Carry flag needs to be set
            (((nVal & 0x0F) + (rVal & 0x0F) + this->getC()) > 0x0F) ? this->setH(true) : this->setH(false);
            // Perform addition to A register
            this->registers[A] += nVal + this->getC();
            // Calculate if Full-Carry flag needs to be set
            (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
            // Calculate if Zero flag needs to be set
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
        else {
            uint8_t nVal = this->registers[r];

            // Calculate if Half-Carry flag needs to be set
            (((nVal & 0x0F) + (rVal & 0x0F) + this->getC()) > 0x0F) ? this->setH(true) : this->setH(false);
            // Perform addition to A register
            this->registers[A] += nVal + this->getC();
            // Calculate if Full-Carry flag needs to be set
            (this->registers[A] < nVal || this->registers[A] < rVal) ? this->setC(true) : this->setC(false);
            // Calculate if Zero flag needs to be set
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
    }
    this->setN(false);
}

void CPU::SUB(uint8_t instruction) {
    std::cout << "SUB A " << std::endl;

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
    std::cout << "SBC A " << std::endl;

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
    // this is incorrect
    ((nVal & 0x0F) + this->getC() > (rVal & 0x0F)) ? this->setH(true) : this->setH(false);
    // Perform subtraction to A register
    this->registers[A] -= nVal + this->getC();
    // Calculate if Full-Carry flag needs to be set
    (nVal + this->getC() > rVal) ? this->setC(true) : this->setC(false);
    // Calculate if Zero flag needs to be set
    (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);

    this->setN(true);
}

void CPU::AND(uint8_t instruction) {
    ;
}

void CPU::XOR(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b11000000) >> 6;
    if (encoding == 0x03) {
        uint8_t nVal = this->addressBus[++(this->PC)];
        this->registers[A] ^= nVal;
        (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
    }
    else {
        uint8_t r = instruction & 0b00000111;
        if (r == 0x06) {
            this->registers[A] ^= this->addressBus[this->getHL()];
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
        else {
            this->registers[A] ^= this->registers[r];
            (this->registers[A] == 0) ? this->setZ(true) : this->setZ(false);
        }
    }
}

void CPU::OR(uint8_t instruction) {
    ;
}

void CPU::CP(uint8_t instruction) {
    ;
}

void CPU::debug() {
    printf("REGS: \nA: %X F: %X\nB: %X C: %X\nD: %X E: %X\nH: %X L: %X\nPC: %X\nSP: %X\n", 
        this->registers[A], this->registers[F], this->registers[B], this->registers[C],
        this->registers[D], this->registers[E], this->registers[H], this->registers[L],
        this->PC, this->SP);
}

void x(...) {
    ;
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
    uint8_t hVal = (uint8_t)value >> 8;
    uint8_t lVal = (uint8_t)value & 0x00FF;
    this->registers[B] = hVal;
    this->registers[C] = lVal;
}

void CPU::setDE(uint16_t value) {
    uint8_t hVal = (uint8_t)value >> 8;
    uint8_t lVal = (uint8_t)value & 0x00FF;
    this->registers[D] = hVal;
    this->registers[E] = lVal;
}

void CPU::setHL(uint16_t value) {
    uint8_t hVal = (uint8_t)value >> 8;
    uint8_t lVal = (uint8_t)value & 0x00FF;
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
    {0X08, &CPU::nop},
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
    {0XC1, &CPU::nop},
    {0XC2, &CPU::nop},
    {0XC3, &CPU::nop},
    {0XC4, &CPU::nop},
    {0XC5, &CPU::nop},
    {0XC6, &CPU::ADD},
    {0XC7, &CPU::nop},
    {0XC8, &CPU::nop},
    {0XC9, &CPU::nop},
    {0XCA, &CPU::nop},
    {0XCB, &CPU::nop},
    {0XCC, &CPU::nop},
    {0XCD, &CPU::nop},
    {0XCE, &CPU::ADC},
    {0XCF, &CPU::nop},
    {0XD0, &CPU::nop},
    {0XD1, &CPU::nop},
    {0XD2, &CPU::nop},
    {0XD3, &CPU::nop},
    {0XD4, &CPU::nop},
    {0XD5, &CPU::nop},
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
    {0XE0, &CPU::nop},
    {0XE1, &CPU::nop},
    {0XE2, &CPU::nop},
    {0XE3, &CPU::nop},
    {0XE4, &CPU::nop},
    {0XE5, &CPU::nop},
    {0XE6, &CPU::AND},
    {0XE7, &CPU::nop},
    {0XE8, &CPU::nop},
    {0XE9, &CPU::nop},
    {0XEA, &CPU::nop},
    {0XEB, &CPU::nop},
    {0XEC, &CPU::nop},
    {0XED, &CPU::nop},
    {0XEE, &CPU::XOR},
    {0XEF, &CPU::nop},
    {0XF0, &CPU::nop},
    {0XF1, &CPU::nop},
    {0XF2, &CPU::nop},
    {0XF3, &CPU::nop},
    {0XF4, &CPU::nop},
    {0XF5, &CPU::nop},
    {0XF6, &CPU::OR},
    {0XF7, &CPU::nop},
    {0XF8, &CPU::nop},
    {0XF9, &CPU::nop},
    {0XFA, &CPU::nop},
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
    {0X40, &CPU::nop},
    {0X41, &CPU::nop},
    {0X42, &CPU::nop},
    {0X43, &CPU::nop},
    {0X44, &CPU::nop},
    {0X45, &CPU::nop},
    {0X46, &CPU::nop},
    {0X47, &CPU::nop},
    {0X48, &CPU::nop},
    {0X49, &CPU::nop},
    {0X4A, &CPU::nop},
    {0X4B, &CPU::nop},
    {0X4C, &CPU::nop},
    {0X4D, &CPU::nop},
    {0X4E, &CPU::nop},
    {0X4F, &CPU::nop},
    {0X50, &CPU::nop},
    {0X51, &CPU::nop},
    {0X52, &CPU::nop},
    {0X53, &CPU::nop},
    {0X54, &CPU::nop},
    {0X55, &CPU::nop},
    {0X56, &CPU::nop},
    {0X57, &CPU::nop},
    {0X58, &CPU::nop},
    {0X59, &CPU::nop},
    {0X5A, &CPU::nop},
    {0X5B, &CPU::nop},
    {0X5C, &CPU::nop},
    {0X5D, &CPU::nop},
    {0X5E, &CPU::nop},
    {0X5F, &CPU::nop},
    {0X60, &CPU::nop},
    {0X61, &CPU::nop},
    {0X62, &CPU::nop},
    {0X63, &CPU::nop},
    {0X64, &CPU::nop},
    {0X65, &CPU::nop},
    {0X66, &CPU::nop},
    {0X67, &CPU::nop},
    {0X68, &CPU::nop},
    {0X69, &CPU::nop},
    {0X6A, &CPU::nop},
    {0X6B, &CPU::nop},
    {0X6C, &CPU::nop},
    {0X6D, &CPU::nop},
    {0X6E, &CPU::nop},
    {0X6F, &CPU::nop},
    {0X70, &CPU::nop},
    {0X71, &CPU::nop},
    {0X72, &CPU::nop},
    {0X73, &CPU::nop},
    {0X74, &CPU::nop},
    {0X75, &CPU::nop},
    {0X76, &CPU::nop},
    {0X77, &CPU::nop},
    {0X78, &CPU::nop},
    {0X79, &CPU::nop},
    {0X7A, &CPU::nop},
    {0X7B, &CPU::nop},
    {0X7C, &CPU::nop},
    {0X7D, &CPU::nop},
    {0X7E, &CPU::nop},
    {0X7F, &CPU::nop},
    {0X80, &CPU::nop},
    {0X81, &CPU::nop},
    {0X82, &CPU::nop},
    {0X83, &CPU::nop},
    {0X84, &CPU::nop},
    {0X85, &CPU::nop},
    {0X86, &CPU::nop},
    {0X87, &CPU::nop},
    {0X88, &CPU::nop},
    {0X89, &CPU::nop},
    {0X8A, &CPU::nop},
    {0X8B, &CPU::nop},
    {0X8C, &CPU::nop},
    {0X8D, &CPU::nop},
    {0X8E, &CPU::nop},
    {0X8F, &CPU::nop},
    {0X90, &CPU::nop},
    {0X91, &CPU::nop},
    {0X92, &CPU::nop},
    {0X93, &CPU::nop},
    {0X94, &CPU::nop},
    {0X95, &CPU::nop},
    {0X96, &CPU::nop},
    {0X97, &CPU::nop},
    {0X98, &CPU::nop},
    {0X99, &CPU::nop},
    {0X9A, &CPU::nop},
    {0X9B, &CPU::nop},
    {0X9C, &CPU::nop},
    {0X9D, &CPU::nop},
    {0X9E, &CPU::nop},
    {0X9F, &CPU::nop},
    {0XA0, &CPU::nop},
    {0XA1, &CPU::nop},
    {0XA2, &CPU::nop},
    {0XA3, &CPU::nop},
    {0XA4, &CPU::nop},
    {0XA5, &CPU::nop},
    {0XA6, &CPU::nop},
    {0XA7, &CPU::nop},
    {0XA8, &CPU::nop},
    {0XA9, &CPU::nop},
    {0XAA, &CPU::nop},
    {0XAB, &CPU::nop},
    {0XAC, &CPU::nop},
    {0XAD, &CPU::nop},
    {0XAE, &CPU::nop},
    {0XAF, &CPU::nop},
    {0XB0, &CPU::nop},
    {0XB1, &CPU::nop},
    {0XB2, &CPU::nop},
    {0XB3, &CPU::nop},
    {0XB4, &CPU::nop},
    {0XB5, &CPU::nop},
    {0XB6, &CPU::nop},
    {0XB7, &CPU::nop},
    {0XB8, &CPU::nop},
    {0XB9, &CPU::nop},
    {0XBA, &CPU::nop},
    {0XBB, &CPU::nop},
    {0XBC, &CPU::nop},
    {0XBD, &CPU::nop},
    {0XBE, &CPU::nop},
    {0XBF, &CPU::nop},
    {0XC0, &CPU::nop},
    {0XC1, &CPU::nop},
    {0XC2, &CPU::nop},
    {0XC3, &CPU::nop},
    {0XC4, &CPU::nop},
    {0XC5, &CPU::nop},
    {0XC6, &CPU::nop},
    {0XC7, &CPU::nop},
    {0XC8, &CPU::nop},
    {0XC9, &CPU::nop},
    {0XCA, &CPU::nop},
    {0XCB, &CPU::nop},
    {0XCC, &CPU::nop},
    {0XCD, &CPU::nop},
    {0XCE, &CPU::nop},
    {0XCF, &CPU::nop},
    {0XD0, &CPU::nop},
    {0XD1, &CPU::nop},
    {0XD2, &CPU::nop},
    {0XD3, &CPU::nop},
    {0XD4, &CPU::nop},
    {0XD5, &CPU::nop},
    {0XD6, &CPU::nop},
    {0XD7, &CPU::nop},
    {0XD8, &CPU::nop},
    {0XD9, &CPU::nop},
    {0XDA, &CPU::nop},
    {0XDB, &CPU::nop},
    {0XDC, &CPU::nop},
    {0XDD, &CPU::nop},
    {0XDE, &CPU::nop},
    {0XDF, &CPU::nop},
    {0XE0, &CPU::nop},
    {0XE1, &CPU::nop},
    {0XE2, &CPU::nop},
    {0XE3, &CPU::nop},
    {0XE4, &CPU::nop},
    {0XE5, &CPU::nop},
    {0XE6, &CPU::nop},
    {0XE7, &CPU::nop},
    {0XE8, &CPU::nop},
    {0XE9, &CPU::nop},
    {0XEA, &CPU::nop},
    {0XEB, &CPU::nop},
    {0XEC, &CPU::nop},
    {0XED, &CPU::nop},
    {0XEE, &CPU::nop},
    {0XEF, &CPU::nop},
    {0XF0, &CPU::nop},
    {0XF1, &CPU::nop},
    {0XF2, &CPU::nop},
    {0XF3, &CPU::nop},
    {0XF4, &CPU::nop},
    {0XF5, &CPU::nop},
    {0XF6, &CPU::nop},
    {0XF7, &CPU::nop},
    {0XF8, &CPU::nop},
    {0XF9, &CPU::nop},
    {0XFA, &CPU::nop},
    {0XFB, &CPU::nop},
    {0XFC, &CPU::nop},
    {0XFD, &CPU::nop},
    {0XFE, &CPU::nop},
    {0XFF, &CPU::nop},
};
