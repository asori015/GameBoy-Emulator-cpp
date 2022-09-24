#include <stdio.h>
#include <fstream>
#include "cpu.h"
#include "bootroms.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(Machine* machine) {
    this->machine_ = machine;
    this->PC_ = 0;
    this->SP_ = 0;
    this->debug_ = true;
    this->loadGameROM("");
    this->loadBIOS(BootROMs::BIOS_DMG, 256, 0);
    //this->loadBIOS(BootROMs::BIOS_CGB, 2048, 0);
    this->run();

    //this->step();
}

void CPU::loadBIOS(const uint8_t* ROM, int size, uint16_t address) {
    for (uint16_t i = 0; i < size; i++) {
        addressBus_[address + i] = ROM[i];
    }
}

void CPU::loadGameROM(std::string filePath) {
    filePath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
    std::ifstream gameFile(filePath, std::ios::binary);
    gameFile.read((char*)(addressBus_), 0x7FFF);
}

// This function is temporary, step function should be called by the Machine class
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
    uint8_t instruction = addressBus_[PC_];
    execute(instruction);
    if (debug_) { printRegs(); }
}

void CPU::execute(uint8_t instruction) {
    uint8_t opcode = (instruction & 0b11000000) >> 6;
    uint8_t register1 = (instruction & 0b00111000) >> 3;
    uint8_t register2 = (instruction & 0b00000111);
    (this->*instructionMethods1_[instruction])(opcode, register1, register2);
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
        printf("ADC ");
    }
    else {
        printf("ADD ");
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
        printf("SBC ");
    }
    else {
        printf("SUB ");
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
    case 0x00:
        setBC(getBC() - 1);
        if (debug_) { printf("DEC BC\n"); }
        break;
    case 0x02:
        setDE(getDE() - 1);
        if (debug_) { printf("DEC DE\n"); }
        break;
    case 0x04:
        setHL(getHL() - 1);
        if (debug_) { printf("DEC HL\n"); }
        break;
    case 0x06:
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
        if (debug_) { printf("RET\n"); }
    }

    uint8_t hAddr = addressBus_[SP_++];
    uint8_t lAddr = addressBus_[SP_++];

    PC_ = (hAddr << 8) + lAddr;
}

void CPU::printRegs() {
    printf("REGS: \nA: 0x%02X F: 0x%02X\nB: 0x%02X C: 0x%02X\nD: 0x%02X E: 0x%02X\nH: 0x%02X L: 0x%02X\nPC: 0x%04X\nSP: 0x%04X\n\n", 
        registers_[A], registers_[F], registers_[B], registers_[C],
        registers_[D], registers_[E], registers_[H], registers_[L],
        PC_, SP_);
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

std::map<uint8_t, CPU::FunctionPointer> CPU::instructionMethods1_ = { 
    {0X00, &CPU::nop},
    {0X01, &CPU::LD_16_Bit},
    {0X02, &CPU::LD_8_Bit},
    {0X03, &CPU::INC_16_BIT},
    {0X04, &CPU::INC},
    {0X05, &CPU::DEC},
    {0X06, &CPU::LD_8_Bit},
    {0X07, &CPU::RLC},
    {0X08, &CPU::LD_16_Bit},
    {0X09, &CPU::ADD_16_BIT},
    {0X0A, &CPU::LD_8_Bit},
    {0X0B, &CPU::DEC_16_BIT},
    {0X0C, &CPU::INC},
    {0X0D, &CPU::DEC},
    {0X0E, &CPU::LD_8_Bit},
    {0X0F, &CPU::RRC},
    {0X10, &CPU::nop},
    {0X11, &CPU::LD_16_Bit},
    {0X12, &CPU::LD_8_Bit},
    {0X13, &CPU::INC_16_BIT},
    {0X14, &CPU::INC},
    {0X15, &CPU::DEC},
    {0X16, &CPU::LD_8_Bit},
    {0X17, &CPU::RL},
    {0X18, &CPU::JR},
    {0X19, &CPU::ADD_16_BIT},
    {0X1A, &CPU::LD_8_Bit},
    {0X1B, &CPU::DEC_16_BIT},
    {0X1C, &CPU::INC},
    {0X1D, &CPU::DEC},
    {0X1E, &CPU::LD_8_Bit},
    {0X1F, &CPU::RR},
    {0X20, &CPU::JR},
    {0X21, &CPU::LD_16_Bit},
    {0X22, &CPU::LD_8_Bit},
    {0X23, &CPU::INC_16_BIT},
    {0X24, &CPU::INC},
    {0X25, &CPU::DEC},
    {0X26, &CPU::LD_8_Bit},
    {0X27, &CPU::nop},
    {0X28, &CPU::JR},
    {0X29, &CPU::ADD_16_BIT},
    {0X2A, &CPU::LD_8_Bit},
    {0X2B, &CPU::DEC_16_BIT},
    {0X2C, &CPU::INC},
    {0X2D, &CPU::DEC},
    {0X2E, &CPU::LD_8_Bit},
    {0X2F, &CPU::nop},
    {0X30, &CPU::JR},
    {0X31, &CPU::LD_16_Bit},
    {0X32, &CPU::LD_8_Bit},
    {0X33, &CPU::INC_16_BIT},
    {0X34, &CPU::INC},
    {0X35, &CPU::DEC},
    {0X36, &CPU::LD_8_Bit},
    {0X37, &CPU::nop},
    {0X38, &CPU::JR},
    {0X39, &CPU::ADD_16_BIT},
    {0X3A, &CPU::LD_8_Bit},
    {0X3B, &CPU::DEC_16_BIT},
    {0X3C, &CPU::INC},
    {0X3D, &CPU::DEC},
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
    {0X88, &CPU::ADD},
    {0X89, &CPU::ADD},
    {0X8A, &CPU::ADD},
    {0X8B, &CPU::ADD},
    {0X8C, &CPU::ADD},
    {0X8D, &CPU::ADD},
    {0X8E, &CPU::ADD},
    {0X8F, &CPU::ADD},
    {0X90, &CPU::SUB},
    {0X91, &CPU::SUB},
    {0X92, &CPU::SUB},
    {0X93, &CPU::SUB},
    {0X94, &CPU::SUB},
    {0X95, &CPU::SUB},
    {0X96, &CPU::SUB},
    {0X97, &CPU::SUB},
    {0X98, &CPU::SUB},
    {0X99, &CPU::SUB},
    {0X9A, &CPU::SUB},
    {0X9B, &CPU::SUB},
    {0X9C, &CPU::SUB},
    {0X9D, &CPU::SUB},
    {0X9E, &CPU::SUB},
    {0X9F, &CPU::SUB},
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
    {0XC0, &CPU::RET},
    {0XC1, &CPU::LD_16_Bit},
    {0XC2, &CPU::JP},
    {0XC3, &CPU::JP},
    {0XC4, &CPU::CALL},
    {0XC5, &CPU::LD_16_Bit},
    {0XC6, &CPU::ADD},
    {0XC7, &CPU::nop},
    {0XC8, &CPU::RET},
    {0XC9, &CPU::RET},
    {0XCA, &CPU::JP},
    {0XCB, &CPU::CBPrefix},
    {0XCC, &CPU::CALL},
    {0XCD, &CPU::CALL},
    {0XCE, &CPU::ADD},
    {0XCF, &CPU::nop},
    {0XD0, &CPU::RET},
    {0XD1, &CPU::LD_16_Bit},
    {0XD2, &CPU::JP},
    {0XD3, &CPU::nop},
    {0XD4, &CPU::CALL},
    {0XD5, &CPU::LD_16_Bit},
    {0XD6, &CPU::SUB},
    {0XD7, &CPU::nop},
    {0XD8, &CPU::RET},
    {0XD9, &CPU::RET},
    {0XDA, &CPU::JP},
    {0XDB, &CPU::nop},
    {0XDC, &CPU::CALL},
    {0XDD, &CPU::nop},
    {0XDE, &CPU::SUB},
    {0XDF, &CPU::nop},
    {0XE0, &CPU::LD_8_Bit},
    {0XE1, &CPU::LD_16_Bit},
    {0XE2, &CPU::LD_8_Bit},
    {0XE3, &CPU::nop},
    {0XE4, &CPU::nop},
    {0XE5, &CPU::LD_16_Bit},
    {0XE6, &CPU::AND},
    {0XE7, &CPU::nop},
    {0XE8, &CPU::ADD_16_BIT},
    {0XE9, &CPU::JP},
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

std::map<uint8_t, CPU::FunctionPointer> CPU::instructionMethods2_ = {
    {0X00, &CPU::RLC},
    {0X01, &CPU::RLC},
    {0X02, &CPU::RLC},
    {0X03, &CPU::RLC},
    {0X04, &CPU::RLC},
    {0X05, &CPU::RLC},
    {0X06, &CPU::RLC},
    {0X07, &CPU::RLC},
    {0X08, &CPU::RRC},
    {0X09, &CPU::RRC},
    {0X0A, &CPU::RRC},
    {0X0B, &CPU::RRC},
    {0X0C, &CPU::RRC},
    {0X0D, &CPU::RRC},
    {0X0E, &CPU::RRC},
    {0X0F, &CPU::RRC},
    {0X10, &CPU::RL},
    {0X11, &CPU::RL},
    {0X12, &CPU::RL},
    {0X13, &CPU::RL},
    {0X14, &CPU::RL},
    {0X15, &CPU::RL},
    {0X16, &CPU::RL},
    {0X17, &CPU::RL},
    {0X18, &CPU::RR},
    {0X19, &CPU::RR},
    {0X1A, &CPU::RR},
    {0X1B, &CPU::RR},
    {0X1C, &CPU::RR},
    {0X1D, &CPU::RR},
    {0X1E, &CPU::RR},
    {0X1F, &CPU::RR},
    {0X20, &CPU::SLA},
    {0X21, &CPU::SLA},
    {0X22, &CPU::SLA},
    {0X23, &CPU::SLA},
    {0X24, &CPU::SLA},
    {0X25, &CPU::SLA},
    {0X26, &CPU::SLA},
    {0X27, &CPU::SLA},
    {0X28, &CPU::SRA},
    {0X29, &CPU::SRA},
    {0X2A, &CPU::SRA},
    {0X2B, &CPU::SRA},
    {0X2C, &CPU::SRA},
    {0X2D, &CPU::SRA},
    {0X2E, &CPU::SRA},
    {0X2F, &CPU::SRA},
    {0X30, &CPU::SWAP},
    {0X31, &CPU::SWAP},
    {0X32, &CPU::SWAP},
    {0X33, &CPU::SWAP},
    {0X34, &CPU::SWAP},
    {0X35, &CPU::SWAP},
    {0X36, &CPU::SWAP},
    {0X37, &CPU::SWAP},
    {0X38, &CPU::SRL},
    {0X39, &CPU::SRL},
    {0X3A, &CPU::SRL},
    {0X3B, &CPU::SRL},
    {0X3C, &CPU::SRL},
    {0X3D, &CPU::SRL},
    {0X3E, &CPU::SRL},
    {0X3F, &CPU::SRL},
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
