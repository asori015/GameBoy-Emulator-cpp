#include "cpu.h"
#include "bootroms.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->registers = new Registers();
    this->PC = 0;
    this->SP = 0;
    this->loadBIOS(0);
    this->run();

    //AddressBus[0] = 0x46;
    //registers->REGS[4] = 0xff;
    //registers->REGS[5] = 0xff;

    /*for (int i = 0; i < 256; i++) {
        std::cout << "{0x" << std::hex << i << ", &CPU::todo}," << std::endl;
    }*/

    //this->step();
}

void CPU::loadBIOS(uint16_t address) {
    const uint8_t* ROM = BootROMs::BIOS_DMG;
    for (uint16_t i = 0; i < 256; i++) {
        AddressBus[address + i] = ROM[i];
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
    uint8_t instruction = this->AddressBus[this->PC];
    execute(instruction);
    //std::cout << "In Step";
}

void CPU::execute(uint8_t instruction) {
    // uint8_t opCode = instruction & 0xC0; // look at two most significant bits
    (this->*InstructionMethods1[instruction])(instruction);
    this->PC += 1;
}

void CPU::LD_16_Bit(uint8_t instruction) {
    uint8_t encoding = (instruction & 0b00110000) >> 4;
    uint8_t lVal = this->AddressBus[++(this->PC)];
    uint8_t hVal = this->AddressBus[++(this->PC)];
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

void CPU::Handle_00_Opcodes(uint8_t instruction) {
    uint8_t r1 = instruction & 0x07;
    uint8_t r2 = (instruction & 0x38) >> 3;
    /*if (r1 == 0x01) {
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
    }*/
    if (r1 == 0x02) {
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
    std::cout << "In Add" << std::endl;
}

void CPU::nop(uint8_t instruction) {
    std::cout << std::hex << int(instruction) << " nop" << std::endl;
}

void CPU::setAF(uint8_t hVal, uint8_t lVal) {
    this->registers->REGS[this->registers->A] = hVal;
    this->registers->REGS[this->registers->F] = lVal;
}

void CPU::setBC(uint8_t hVal, uint8_t lVal) {
    this->registers->REGS[this->registers->B] = hVal;
    this->registers->REGS[this->registers->C] = lVal;
}

void CPU::setDE(uint8_t hVal, uint8_t lVal) {
    this->registers->REGS[this->registers->D] = hVal;
    this->registers->REGS[this->registers->E] = lVal;
}

void CPU::setHL(uint8_t hVal, uint8_t lVal) {
    this->registers->REGS[this->registers->H] = hVal;
    this->registers->REGS[this->registers->L] = lVal;
}

void CPU::setAF(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers->REGS[this->registers->A] = hVal;
    this->registers->REGS[this->registers->F] = lVal;
}

void CPU::setBC(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers->REGS[this->registers->B] = hVal;
    this->registers->REGS[this->registers->C] = lVal;
}

void CPU::setDE(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers->REGS[this->registers->D] = hVal;
    this->registers->REGS[this->registers->E] = lVal;
}

void CPU::setHL(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    this->registers->REGS[this->registers->H] = hVal;
    this->registers->REGS[this->registers->L] = lVal;
}

std::map<uint8_t, CPU::functionPointer> CPU::InstructionMethods1 = { 
    {0X00, &CPU::nop},
    {0X01, &CPU::LD_16_Bit},
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
    {0X11, &CPU::LD_16_Bit},
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
    {0X21, &CPU::LD_16_Bit},
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
    {0X31, &CPU::LD_16_Bit},
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
