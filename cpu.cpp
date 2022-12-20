#include <stdio.h>
#include <fstream>
#include "cpu.h"
#include "bootroms.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine, uint8_t* addressBus) {
    this->machine_ = machine;
    this->PC_ = 0;
    this->SP_ = 0;
    this->clock_ = 0;
    this->isHalted_ = false;
    //this->debug_ = true;
    this->addressBus_ = addressBus;
    this->loadGameROM("");
    this->loadBIOS(BootROMs::BIOS_DMG, 256, 0);
    //this->loadBIOS(BootROMs::BIOS_CGB, 2048, 0);
    //this->run();

    //this->step();
}

void CPU::loadBIOS(const uint8_t* ROM, int size, uint16_t address) {
    for (uint16_t i = 0; i < size; i++) {
        addressBus_[address + i] = ROM[i];
    }
}

void CPU::loadGameROM(std::string filePath) {
    //filePath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    //filePath = "D:\\Games\\GBA\\Tetris\\Tetris.gb";
    filePath = "D:\\Games\\GBA\\dmg-acid2.gb";
    //filePath = "D:\\Games\\GBA\\cpu_instrs.gb";
    //filePath = "D:\\Games\\GBA\\01-special.gb";
    //filePath = "D:\\Games\\GBA\\06-ld r,r.gb";
    std::ifstream gameFile(filePath, std::ios::binary);
    gameFile.read((char*)(addressBus_), 0x7FFF);
    gameFile.close();
}

// This function is temporary, step function should be called by the Machine class
// Function Deprecated...
void CPU::run() {
    debug_ = false;
    while (this->PC_ != 0x0068) {
        step();
    }

    //debug_ = true;
    //int x = 0; // Temporary set up, this loop should be infinite
    //while (x < 32) {
    //    step();
    //    x++;
    //}
}

void CPU::step() {
    if (clock_ == 0) {
        if (addressBus_[0xFF0F]) {
            uint8_t mask = 0x01;
            for (int i = 0; i < 5; i++) {
                if ((addressBus_[0xFF0F] & mask) && (addressBus_[0xFFFF] & mask)) {
                    isHalted_ = false;

                    if (IME_) {
                        //printf("test");
                        IME_ = false;
                        addressBus_[0xFF0F] &= !mask;
                        addressBus_[--SP_] = 0x00FF & PC_;
                        addressBus_[--SP_] = (0xFF00 & PC_) >> 8;
                        PC_ = 0x0040 + (8 * i);
                        clock_ += 4;
                        //debug_ = true;
                    }
                    break;
                }
                mask = mask << 1;
                
            }
        }
        if (!isHalted_) {
            execute(addressBus_[PC_]);
        }
        
    }
    else {
        clock_ -= 1;
    }
}

void CPU::execute(uint8_t instruction) {
    /*if (PC_ == 0x1F72) {
        debug_ = true;
    }*/

    if (PC_ == 0xC000) {
        //debug_ = true;
    }

    if (PC_ == 0x0100) {
        loadGameROM("");
        //debug_ = true;
    }

    /*if (instruction == 0x00) {
        debug_ = true;
    }*/

    uint8_t opcode = (instruction & 0b11000000) >> 6;
    uint8_t register1 = (instruction & 0b00111000) >> 3;
    uint8_t register2 = (instruction & 0b00000111);
    (this->*instructionMethods1_[instruction])(opcode, register1, register2);
    clock_ = 4;//instructionMethods1_[instruction].second - 1;
    if (debug_) { printf("Instruction: 0x%x\n", instruction); printRegs(); }
    PC_ += 1;
}

void CPU::LD_R_to_R(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        if (reg1 == 0x06) {
            // HALT INSTRUCTION
        }
        else {
            registers_[reg1] = addressBus_[getHL()];
            if (debug_) { printf("LD %c, (HL)\n", regNames_[reg1]); }
        }
    }
    else {
        if (reg1 == 0x06) {
            addressBus_[getHL()] = registers_[reg2];
            if (debug_) { printf("LD (HL), %c\n", regNames_[reg2]); }
        }
        else {
            registers_[reg1] = registers_[reg2];
            clock_ += 4;
            if (debug_) { printf("LD %c, %c\n", regNames_[reg1], regNames_[reg2]); }
        }
    }
}

void CPU::LD_8_Bit(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (op == 0x03) {
        uint16_t address;
        if (reg2 == 0x00) {
            switch (reg1) {
            case 0x04:
                address = 0xFF00 + addressBus_[++PC_];
                addressBus_[address] = registers_[A];
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + addressBus_[++PC_];
                registers_[A] = addressBus_[address];
                if (debug_) { printf("LD A, (0x%04X)\n", address); }
                break;
            default:
                break;
            }
        }
        else if(reg2 == 0x02) {
            switch (reg1) {
            case 0x04:
                address = 0xFF00 + registers_[C];
                addressBus_[address] = registers_[A];
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x05:
                address = addressBus_[++PC_] + (addressBus_[++PC_] << 8);
                addressBus_[address] = registers_[A];
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + registers_[C];
                registers_[A] = addressBus_[address];
                if (debug_) { printf("LD A, (0x%04X)\n", address); }
                break;
            case 0x07:
                address = addressBus_[++PC_] + (addressBus_[++PC_] << 8);
                registers_[A] = addressBus_[address];
                if (debug_) { printf("LD A, (0x%04X)\n", address); }
                break;
            default:
                break;
            }
        }
    }
    else {
        if (reg2 == 0x02) {
            uint16_t hl = getHL();
            switch (reg1)
            {
            case 0x00:
                addressBus_[getBC()] = registers_[A];
                if (debug_) { printf("LD (BC), A\n"); }
                break;
            case 0x01:
                registers_[A] = addressBus_[getBC()];
                if (debug_) { printf("LD A, (BC)\n"); }
                break;
            case 0x02:
                addressBus_[getDE()] = registers_[A];
                if (debug_) { printf("LD (DE), A\n"); }
                break;
            case 0x03:
                registers_[A] = addressBus_[getDE()];
                if (debug_) { printf("LD A, (DE)\n"); }
                break;
            case 0x04:
                addressBus_[hl] = registers_[A];
                setHL(hl + 1);
                if (debug_) { printf("LD (HL+), A\n"); }
                break;
            case 0x05:
                registers_[A] = addressBus_[hl];
                setHL(hl + 1);
                if (debug_) { printf("LD A, (HL+)\n"); }
                break;
            case 0x06:
                addressBus_[hl] = registers_[A];
                setHL(hl - 1);
                if (debug_) { printf("LD (HL-), A\n"); }
                break;
            case 0x07:
                registers_[A] = addressBus_[hl];
                setHL(hl - 1);
                if (debug_) { printf("LD A, (HL-)\n"); }
                break;
            default:
                break;
            }
        }
        else if (reg2 == 0x06) {
            uint8_t val = addressBus_[++PC_];
            if (reg1 == 0x06) {
                addressBus_[getHL()] = val;
                if (debug_) { printf("LD (HL), 0x%02X\n", val); }
            }
            else {
                registers_[reg1] = val;
                if (debug_) { printf("LD %c, 0x%02X\n", regNames_[reg1], val); }
            }
        }
    }
}

void CPU::LD_16_Bit(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (op == 0x03) {
        if (reg2 == 0x01) {
            uint8_t lVal;
            uint8_t hVal;
            switch (reg1)
            {
            case 0x00:
                lVal = addressBus_[SP_++];
                hVal = addressBus_[SP_++];
                setBC(hVal, lVal);
                if (debug_) { printf("POP BC\n"); }
                break;
            case 0x02:
                lVal = addressBus_[SP_++];
                hVal = addressBus_[SP_++];
                setDE(hVal, lVal);
                if (debug_) { printf("POP DE\n"); }
                break;
            case 0x04:
                lVal = addressBus_[SP_++];
                hVal = addressBus_[SP_++];
                setHL(hVal, lVal);
                if (debug_) { printf("POP HL\n"); }
                break;
            case 0x06:
                lVal = addressBus_[SP_++];
                hVal = addressBus_[SP_++];
                setAF(hVal, lVal);
                if (debug_) { printf("POP AF\n"); }
                break;
            case 0x07:
                SP_ = getHL();
                if (debug_) { printf("LD SP, HL\n"); }
                break;
            default:
                break;
            }
        }
        else {
            int8_t val;
            uint16_t result;
            switch (reg1)
            {
            case 0x00:
                addressBus_[--SP_] = registers_[B];
                addressBus_[--SP_] = registers_[C];
                if (debug_) { printf("PUSH BC\n"); }
                break;
            case 0x02:
                addressBus_[--SP_] = registers_[D];
                addressBus_[--SP_] = registers_[E];
                if (debug_) { printf("PUSH DE\n"); }
                break;
            case 0x04:
                addressBus_[--SP_] = registers_[H];
                addressBus_[--SP_] = registers_[L];
                if (debug_) { printf("PUSH HL\n"); }
                break;
            case 0x06:
                addressBus_[--SP_] = registers_[A];
                addressBus_[--SP_] = registers_[F];
                if (debug_) { printf("PUSH AF\n"); }
                break;
            case 0x07:
                val = addressBus_[++PC_];
                result = SP_ + val;
                setHL(result);

                // Calculate if Carry flag needs to be set
                setC(val > 0 && result < SP_);
                // Calculate if Half-Carry flag needs to be set
                setH(val > 0 && (result & 0x0FFF) < (SP_ & 0xFFF));
                // Set N and Z flags to 0
                setN(false);
                setZ(false);

                if (debug_) { printf("HL, SP+%d\n", val); }
                break;
            default:
                break;
            }
        }
    }
    else {
        uint8_t lVal = addressBus_[++PC_];
        uint8_t hVal = addressBus_[++PC_];
        uint16_t addr;
        switch (reg1)
        {
        case 0x00:
            setBC(hVal, lVal);
            if (debug_) { printf("LD BC, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x01:
            addr = (hVal << 8) + lVal;
            addressBus_[addr] = getSP() & 0x00FF;
            addressBus_[addr + 1] = (getSP() & 0xFF00) >> 8;
            if (debug_) { printf("LD (0x%04X), SP\n", addr); }
            break;
        case 0x02:
            setDE(hVal, lVal);
            if (debug_) { printf("LD DE, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x04:
            setHL(hVal, lVal);
            if (debug_) { printf("LD HL, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x06:
            SP_ = (hVal << 8) + lVal;
            if (debug_) { printf("LD SP, 0x%02X%02X\n", hVal, lVal); }
            break;
        default:
            break;
        }
    }
}

void CPU::JP(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x01) {
        PC_ = getHL() - 1;
        if (debug_) { printf("JP HL\n"); }
    }
    else {
        uint8_t lVal = addressBus_[++PC_];
        uint8_t hVal = addressBus_[++PC_];
        uint16_t addr = (hVal << 8) + lVal;

        if (reg2 == 0x02) {
            switch (reg1)
            {
            case 0x00:
                if (debug_) { printf("JP NZ 0x%04X\n", addr); }
                if (getZ()) { return; }
                break;
            case 0x01:
                if (debug_) { printf("JP Z 0x%04X\n", addr); }
                if (!getZ()) { return; }
                break;
            case 0x02:
                if (debug_) { printf("JP NC 0x%04X\n", addr); }
                if (getC()) { return; }
                break;
            case 0x03:
                if (debug_) { printf("JP C 0x%04X\n", addr); }
                if (!getC()) { return; }
                break;
            default:
                break;
            }
        }
        else {
            if (debug_) { printf("JP 0x%04X\n", addr); }
        }

        PC_ = addr - 1;
    }
}

void CPU::JR(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    int8_t nVal = addressBus_[++PC_];

    if (reg1 != 0x03) {
        switch (reg1)
        {
        case 0x04:
            if (debug_) { printf("JR NZ %d\n", nVal); }
            if (getZ()) { return; }
            break;
        case 0x05:
            if (debug_) { printf("JR Z %d\n", nVal); }
            if (!getZ()) { return; }
            break;
        case 0x06:
            if (debug_) { printf("JR NC %d\n", nVal); }
            if (getC()) { return; }
            break;
        case 0x07:
            if (debug_) { printf("JR C %d\n", nVal); }
            if (!getC()) { return; }
            break;
        default:
            break;
        }
    }
    else {
        if (debug_) { printf("JR %d\n", nVal); }
    }
    
    PC_ += nVal;
}

void CPU::ADD(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t rVal = registers_[A];
    uint8_t nVal;
    bool carry = false;

    // Check if carry bit will be used
    if (reg1 == 0x01) {
        carry = getC();
        if (debug_) { printf("ADC "); }
    }
    else {
        //printf("ADD ");
    }

    // Get the value being used for the calculation with Register A
    if (op == 0x03) {
        nVal = addressBus_[++PC_];
        if (debug_) { printf("A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = addressBus_[getHL()];
            if (debug_) { printf("A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            if (debug_) { printf("A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    setH(((nVal & 0x0F) + (rVal & 0x0F) + carry) > 0x0F);
    // Perform addition to A register
    registers_[A] += nVal + carry;
    // Calculate if Full-Carry flag needs to be set
    setC((registers_[A] < rVal));
    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set N flag to 0
    setN(false);
}

void CPU::SUB(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t rVal = registers_[A];
    uint8_t nVal;
    bool carry = false;

    // Check if carry bit will be used
    if (reg1 == 0x03) {
        carry = getC();
        // printf("SBC ");
    }
    else {
        // printf("SUB ");
    }

    // Get the value being used for the calculation with Register A
    if (op == 0x03) {
        nVal = addressBus_[++PC_];
        if (debug_) { printf("A, 0x % 02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = addressBus_[getHL()];
            if (debug_) { printf("A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            if (debug_) { printf("A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    setH((rVal & 0x0F) < (nVal & 0x0F) + carry);
    // Perform subtraction to A register
    registers_[A] -= nVal + carry;
    // Calculate if Full-Carry flag needs to be set
    setC(nVal + carry > rVal);
    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set N flag to 1
    setN(true);
}

void CPU::AND(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (op == 0x03) {
        uint8_t nVal = addressBus_[++PC_];
        registers_[A] &= nVal;
        if (debug_) { printf("AND A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] &= addressBus_[getHL()];
            if (debug_) { printf("AND A, (HL)\n"); }
        }
        else {
            registers_[A] &= registers_[reg2];
            if (debug_) { printf("AND A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set C and N flags to 0, H flag to 1
    setC(false);
    setH(true);
    setN(false);
}

void CPU::XOR(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (op == 0x03) {
        uint8_t nVal = addressBus_[++PC_];
        registers_[A] ^= nVal;
        if (debug_) { printf("XOR A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] ^= addressBus_[getHL()];
            if (debug_) { printf("XOR A, (HL)\n"); }
        }
        else {
            registers_[A] ^= registers_[reg2];
            if (debug_) { printf("XOR A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set C, H, and N flags to 0
    setC(false);
    setH(false);
    setN(false);
}

void CPU::OR(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (op == 0x03) {
        uint8_t nVal = addressBus_[++PC_];
        registers_[A] |= nVal;
        if (debug_) { printf("OR A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] |= addressBus_[getHL()];
            if (debug_) { printf("OR A, (HL)\n"); }
        }
        else {
            registers_[A] |= registers_[reg2];
            if (debug_) { printf("OR A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set C, H, and N flags to 0
    setC(false);
    setH(false);
    setN(false);
}

void CPU::CP(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t rVal = registers_[A];
    uint8_t nVal;

    // Get the value being used for the calculation with Register A
    if (op == 0x03) {
        nVal = addressBus_[++PC_];
        if (debug_) { printf("CP A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = addressBus_[getHL()];
            if (debug_) { printf("CP A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            if (debug_) { printf("CP A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    setH(nVal < rVal);
    // Calculate if Full-Carry flag needs to be set
    setC(nVal > rVal);
    // Calculate if Zero flag needs to be set
    setZ(nVal == rVal);
    // Set N flag to 1
    setN(true);
}

void CPU::INC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t result;

    if (reg1 == 0x06) {
        addressBus_[getHL()] += 1;
        result = addressBus_[getHL()];
        if (debug_) { printf("INC (HL)\n"); }
    }
    else {
        registers_[reg1] += 1;
        result = registers_[reg1];
        if (debug_) { printf("INC %c\n", regNames_[reg1]); }
    }

    // Calculate if Zero flag needs to be set
    setZ(result == 0x00);
    // Calculate if Half-Carry flag needs to be set
    setH((result & 0x0F) == 0x00);
    // Set N flag to 0
    setN(false);
}

void CPU::DEC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t result;

    if (reg1 == 0x06) {
        addressBus_[getHL()] -= 1;
        result = addressBus_[getHL()];
        if (debug_) { printf("DEC (HL)\n"); }
    }
    else {
        registers_[reg1] -= 1;
        result = registers_[reg1];
        if (debug_) { printf("DEC %c\n", regNames_[reg1]); }
    }

    // Calculate if Zero flag needs to be set
    setZ(result == 0x00);
    // Calculate if Half-Carry flag needs to be set
    setH((result & 0x0F) == 0x0F);
    // Set N flag to 1
    setN(true);
}

void CPU::ADD_16_BIT(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x00) {
        int8_t nVal = addressBus_[++PC_]; // Interpreted as signed value
        uint16_t rVal = SP_;
        SP_ += nVal;
        
        // Calculate if Carry flag needs to be set
        setC(nVal > 0x00 && SP_ < rVal);
        // Calculate if Half-Carry flag needs to be set
        setH(nVal > 0x00 && (SP_ & 0x0F00) < (rVal & 0x0F00));
        // Set Z flag to 0
        setZ(false);
        if (debug_) { printf("ADD SP, 0x%02X\n", nVal); }
    }
    else {
        uint16_t rVal = getHL();
        switch (reg1)
        {
        case 0x01:
            setHL(getHL() + getBC());
            if (debug_) { printf("ADD HL, BC\n"); }
            break;
        case 0x03:
            setHL(getHL() + getDE());
            if (debug_) { printf("ADD HL, DE\n"); }
            break;
        case 0x05:
            setHL(getHL() + getHL());
            if (debug_) { printf("ADD HL, HL\n"); }
            break;
        case 0x07:
            setHL(getHL() + getSP());
            if (debug_) { printf("ADD HL, SP\n"); }
            break;
        default:
            break;
        }

        // Calculate if Carry flag needs to be set
        setC(getHL() < rVal);
        // Calculate if Half-Carry flag needs to be set
        setH((getHL() & 0x0F00) < (rVal & 0x0F00));
        // Set N flag to 0
        setN(false);
    }
}

void CPU::INC_16_BIT(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    switch (reg1)
    {
    case 0x00:
        setBC(getBC() + 1);
        if (debug_) { printf("INC BC\n"); }
        break;
    case 0x02:
        setDE(getDE() + 1);
        if (debug_) { printf("INC DE\n"); }
        break;
    case 0x04:
        setHL(getHL() + 1);
        if (debug_) { printf("INC HL\n"); }
        break;
    case 0x06:
        SP_ += 1;
        if (debug_) { printf("INC SP\n"); }
        break;
    default:
        break;
    }
}

void CPU::DEC_16_BIT(uint8_t op, uint8_t reg1, uint8_t reg2) {
    switch (reg1)
    {
    case 0x01:
        setBC(getBC() - 1);
        if (debug_) { printf("DEC BC\n"); }
        break;
    case 0x03:
        setDE(getDE() - 1);
        if (debug_) { printf("DEC DE\n"); }
        break;
    case 0x05:
        setHL(getHL() - 1);
        if (debug_) { printf("DEC HL\n"); }
        break;
    case 0x07:
        SP_ -= 1;
        if (debug_) { printf("DEC SP\n"); }
        break;
    default:
        break;
    }
}

void CPU::CBPrefix(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (debug_) { printf("CB Prefix\n"); }

    uint8_t instruction = addressBus_[++PC_];
    uint8_t opcode = (instruction & 0b11000000) >> 6;
    uint8_t register1 = (instruction & 0b00111000) >> 3;
    uint8_t register2 = (instruction & 0b00000111);
    cbPrefx_ = true;
    (this->*instructionMethods2_[instruction])(opcode, register1, register2);
    cbPrefx_ = false;
}

void CPU::RLC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        addressBus_[getHL()] = (rVal << 1) + getC();

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("RLC (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        registers_[reg2] = (rVal << 1) + getC();

        // Calculate if Zero flag needs to be set
        if (cbPrefx_) {
            setZ(registers_[reg2] == 0x00);
            if (debug_) { printf("RLC %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            if (debug_) { printf("RLCA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RRC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        addressBus_[getHL()] = (rVal >> 1) + (getC() << 7);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("RRC (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        registers_[reg2] = (rVal >> 1) + (getC() << 7);

        // Calculate if Zero flag needs to be set
        if (cbPrefx_) {
            setZ(registers_[reg2] == 0x00);
            if (debug_) { printf("RRC %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            if (debug_) { printf("RRCA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        addressBus_[getHL()] = (rVal << 1) + getC();
        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("RL (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        registers_[reg2] = (rVal << 1) + getC();
        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);

        // Calculate if Zero flag needs to be set
        if (cbPrefx_) {
            setZ(registers_[reg2] == 0x00);
            if (debug_) { printf("RL %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            if (debug_) { printf("RLA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RR(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        addressBus_[getHL()] = (rVal >> 1) + (getC() << 7);
        // Calculate if Carry flag needs to be set
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("RR (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        registers_[reg2] = (rVal >> 1) + (getC() << 7);
        // Calculate if Carry flag needs to be set
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        if (cbPrefx_) {
            setZ(registers_[reg2] == 0x00);
            if (debug_) { printf("RR %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            if (debug_) { printf("RRA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SLA(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        addressBus_[getHL()] = rVal << 1;
        
        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("SLA (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        registers_[reg2] = rVal << 1;

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        if (debug_) { printf("SLA %c\n", regNames_[reg2]); }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SRA(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        addressBus_[getHL()] = (rVal >> 1) + (getC() << 7);
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("SRA (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        registers_[reg2] = (rVal >> 1) + (getC() << 7);
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        if (debug_) { printf("SRA %c\n", regNames_[reg2]); }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SWAP(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];
        addressBus_[getHL()] = (rVal << 4) + (rVal >> 4);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("SWAP (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];
        registers_[reg2] = (rVal << 4) + (rVal >> 4);

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        if (debug_) { printf("SWAP %c\n", regNames_[reg2]); }
    }

    // Set C, H and N flags to 0
    setC(false);
    setH(false);
    setN(false);
}

void CPU::SRL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = addressBus_[getHL()];

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        addressBus_[getHL()] = (rVal >> 1);

        // Calculate if Zero flag needs to be set
        setZ(addressBus_[getHL()] == 0x00);
        if (debug_) { printf("SRA (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        registers_[reg2] = (rVal >> 1);

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        if (debug_) { printf("SRA %c\n", regNames_[reg2]); }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::BIT(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t mask = 0x01 << reg1;

    if (reg2 == 0x06) {
        // Calculate if Zero flag needs to be set
        setZ((addressBus_[getHL()] & mask) == 0);
        if (debug_) { printf("BIT %d, (HL)\n", reg1); }
    }
    else {
        // Calculate if Zero flag needs to be set
        setZ((registers_[reg2] & mask) == 0);
        if (debug_) { printf("BIT %d, %c\n", reg1, regNames_[reg2]); }
    }

    // Set H flag to 1, N flag to 0
    setH(true);
    setN(false);
}

void CPU::RES(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t mask = (0x01 << reg1) ^ 0xFF;

    if (reg2 == 0x06) {
        addressBus_[getHL()] &= mask;
        if (debug_) { printf("RES %d, (HL)\n", reg1); }
    }
    else {
        registers_[reg2] &= mask;
        if (debug_) { printf("RES %d, %c\n", reg1, regNames_[reg2]); }
    }
}

void CPU::SET(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t mask = 0x01 << reg1;

    if (reg2 == 0x06) {
        addressBus_[getHL()] |= mask;
        if (debug_) { printf("SET %d, (HL)\n", reg1); }
    }
    else {
        registers_[reg2] |= mask;
        if (debug_) { printf("SET %d, %c\n", reg1, regNames_[reg2]); }
    }
}

void CPU::CALL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t lAddr = addressBus_[++PC_];
    uint8_t hAddr = addressBus_[++PC_];

    if (reg2 == 0x04) {
        switch (reg1)
        {
        case 0x00:
            if (debug_) { printf("CALL NZ 0x%02X%02X\n", hAddr, lAddr); }
            if (getZ()) { return; }
            break;
        case 0x01:
            if (debug_) { printf("CALL Z 0x%02X%02X\n", hAddr, lAddr); }
            if (!getZ()) { return; }
            break;
        case 0x02:
            if (debug_) { printf("CALL NC 0x%02X%02X\n", hAddr, lAddr); }
            if (getC()) { return; }
            break;
        case 0x03:
            if (debug_) { printf("CALL C 0x%02X%02X\n", hAddr, lAddr); }
            if (!getC()) { return; }
            break;
        default:
            break;
        }
    }
    else {
        if (debug_) { printf("CALL 0x%02X%02X\n", hAddr, lAddr); }
    }

    addressBus_[--SP_] = 0x00FF & PC_;
    addressBus_[--SP_] = (0xFF00 & PC_) >> 8;
    PC_ = (hAddr << 8) + lAddr - 1;
}

void CPU::RET(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x00) {
        switch (reg1)
        {
        case 0x00:
            if (debug_) { printf("RET NZ\n"); }
            if (getZ()) { return; }
            break;
        case 0x01:
            if (debug_) { printf("RET Z\n"); }
            if (!getZ()) { return; }
            break;
        case 0x02:
            if (debug_) { printf("RET NC\n"); }
            if (getC()) { return; }
            break;
        case 0x03:
            if (debug_) { printf("RET C\n"); }
            if (!getC()) { return; }
            break;
        default:
            break;
        }
    }
    else {
        if (reg1 == 0x03) {
            IME_ = true;
            if (debug_) { printf("RETI\n"); }
        }
        else {
            if (debug_) { printf("RET\n"); }
        }
    }

    uint8_t hAddr = addressBus_[SP_++];
    uint8_t lAddr = addressBus_[SP_++];

    PC_ = (hAddr << 8) + lAddr;
}

void CPU::RST(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    //setC(true);
    if (debug_) { printf("RST\n"); }
}

void CPU::DAA(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    //setC(true);
    if (debug_) { printf("CCF\n"); }
}

void CPU::CPL(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    registers_[A] = !registers_[A];
    if (debug_) { printf("CPL\n"); }
}

void CPU::SCF(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    setC(true);
    if (debug_) { printf("SCF\n"); }
}

void CPU::CCF(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    setC(!getC());
    if (debug_) { printf("CCF\n"); }
}

void CPU::DI(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    IME_ = false;
    if (debug_) { printf("DI\n"); }
}

void CPU::EI(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    IME_ = true;
    if (debug_) { printf("EI\n"); }
}

void CPU::HALT(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    isHalted_ = true;
    if (debug_) { printf("HALT\n"); }
}


void CPU::STOP(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    // IME = true;
    if (debug_) { printf("STOP\n"); }
}


void CPU::printRegs() {
    printf("REGS: \nA: 0x%02X F: 0x%02X\nB: 0x%02X C: 0x%02X\nD: 0x%02X E: 0x%02X\nH: 0x%02X L: 0x%02X\nPC: 0x%04X\nSP: 0x%04X\n\n", 
        registers_[A], registers_[F], registers_[B], registers_[C],
        registers_[D], registers_[E], registers_[H], registers_[L],
        PC_ + 1, SP_);
}

void CPU::nop(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    instruction = (instruction << 6) + (reg1 << 3) + reg2;
    if (debug_) { printf("NOP 0x%02X\n", instruction); }
}

uint16_t CPU::getAF() {
    return (registers_[A] << 8) + registers_[F];
}

uint16_t CPU::getBC() {
    return (registers_[B] << 8) + registers_[C];
}

uint16_t CPU::getDE() {
    return (registers_[D] << 8) + registers_[E];
}

uint16_t CPU::getHL() {
    return (registers_[H] << 8) + registers_[L];
}

uint16_t CPU::getSP() {
    return SP_;
}

uint16_t CPU::getPC() {
    return PC_;
}

void CPU::setAF(uint8_t hVal, uint8_t lVal) {
    registers_[A] = hVal;
    registers_[F] = lVal;
}

void CPU::setBC(uint8_t hVal, uint8_t lVal) {
    registers_[B] = hVal;
    registers_[C] = lVal;
}

void CPU::setDE(uint8_t hVal, uint8_t lVal) {
    registers_[D] = hVal;
    registers_[E] = lVal;
}

void CPU::setHL(uint8_t hVal, uint8_t lVal) {
    registers_[H] = hVal;
    registers_[L] = lVal;
}

void CPU::setAF(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    registers_[A] = hVal;
    registers_[F] = lVal;
}

void CPU::setBC(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    registers_[B] = hVal;
    registers_[C] = lVal;
}

void CPU::setDE(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    registers_[D] = hVal;
    registers_[E] = lVal;
}

void CPU::setHL(uint16_t value) {
    uint8_t hVal = uint8_t(value >> 8);
    uint8_t lVal = uint8_t(value & 0x00FF);
    registers_[H] = hVal;
    registers_[L] = lVal;
}

bool CPU::getC() {
    return registers_[F] & 0b00010000;
}

bool CPU::getH() {
    return registers_[F] & 0b00100000;
}

bool CPU::getN() {
    return registers_[F] & 0b01000000;
}

bool CPU::getZ() {
    return registers_[F] & 0b10000000;
}

void CPU::setC(bool val) {
    if (val == true) {
        registers_[F] |= 0b00010000;
    }
    else {
        registers_[F] &= 0b11101111;
    }
}

void CPU::setH(bool val) {
    if (val == true) {
        registers_[F] |= 0b00100000;
    }
    else {
        registers_[F] &= 0b11011111;
    }
}

void CPU::setN(bool val) {
    if (val == true) {
        registers_[F] |= 0b01000000;
    }
    else {
        registers_[F] &= 0b10111111;
    }
}

void CPU::setZ(bool val) {
    if (val == true) {
        registers_[F] |= 0b10000000;
    }
    else {
        registers_[F] &= 0b01111111;
    }
}

const CPU::FunctionPointer CPU::instructionMethods1_[256] = {
    &CPU::nop,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::INC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::RLC,
    &CPU::LD_16_Bit,
    &CPU::ADD_16_BIT,
    &CPU::LD_8_Bit,
    &CPU::DEC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::RRC,
    &CPU::STOP,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::INC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::RL,
    &CPU::JR,
    &CPU::ADD_16_BIT,
    &CPU::LD_8_Bit,
    &CPU::DEC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::RR,
    &CPU::JR,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::INC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::DAA,
    &CPU::JR,
    &CPU::ADD_16_BIT,
    &CPU::LD_8_Bit,
    &CPU::DEC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::CPL,
    &CPU::JR,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::INC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::SCF,
    &CPU::JR,
    &CPU::ADD_16_BIT,
    &CPU::LD_8_Bit,
    &CPU::DEC_16_BIT,
    &CPU::INC,
    &CPU::DEC,
    &CPU::LD_8_Bit,
    &CPU::CCF,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::HALT,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::LD_R_to_R,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::ADD,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::SUB,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::AND,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::XOR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::OR,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::CP,
    &CPU::RET,
    &CPU::LD_16_Bit,
    &CPU::JP,
    &CPU::JP,
    &CPU::CALL,
    &CPU::LD_16_Bit,
    &CPU::ADD,
    &CPU::RST,
    &CPU::RET,
    &CPU::RET,
    &CPU::JP,
    &CPU::CBPrefix,
    &CPU::CALL,
    &CPU::CALL,
    &CPU::ADD,
    &CPU::RST,
    &CPU::RET,
    &CPU::LD_16_Bit,
    &CPU::JP,
    &CPU::nop,
    &CPU::CALL,
    &CPU::LD_16_Bit,
    &CPU::SUB,
    &CPU::RST,
    &CPU::RET,
    &CPU::RET,
    &CPU::JP,
    &CPU::nop,
    &CPU::CALL,
    &CPU::nop,
    &CPU::SUB,
    &CPU::RST,
    &CPU::LD_8_Bit,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::nop,
    &CPU::nop,
    &CPU::LD_16_Bit,
    &CPU::AND,
    &CPU::RST,
    &CPU::ADD_16_BIT,
    &CPU::JP,
    &CPU::LD_8_Bit,
    &CPU::nop,
    &CPU::nop,
    &CPU::nop,
    &CPU::XOR,
    &CPU::RST,
    &CPU::LD_8_Bit,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::DI,
    &CPU::nop,
    &CPU::LD_16_Bit,
    &CPU::OR,
    &CPU::RST,
    &CPU::LD_16_Bit,
    &CPU::LD_16_Bit,
    &CPU::LD_8_Bit,
    &CPU::EI,
    &CPU::nop,
    &CPU::nop,
    &CPU::CP,
    &CPU::RST
};

const CPU::FunctionPointer CPU::instructionMethods2_[256] = {
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RLC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RRC,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RL,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::RR,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SLA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SRA,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SWAP,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::SRL,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::BIT,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::RES,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
    &CPU::SET,
};
