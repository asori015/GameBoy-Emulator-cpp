#include "cpu.h"

//typedef void (CPU::* functionPointer)(uint8_t );

CPU::CPU(MMU* mmu) :
    DIV((uint16_t*)mmu->addrBus(0xFF03)),
    TIMA(mmu->addrBus(0xFF05)),
    TMA(mmu->addrBus(0xFF06)),
    TAC(mmu->addrBus(0xFF07)),
    IF(mmu->addrBus(0xFF0F))
{
    this->mmu = mmu;
    this->PC_ = 0;
    this->SP_ = 0;
    this->clock_ = 0;
    this->isHalted_ = false;
    //this->debug_ = true;
    //this->addressBus_ = addressBus;
    //this->loadGameROM("");
    //this->loadBIOS(BootROMs::BIOS_DMG, 256, 0);
    //this->loadBIOS(BootROMs::BIOS_CGB, 2048, 0);
    //this->run();

    //this->step();
}

//void CPU::loadBIOS(const uint8_t* ROM, int size, uint16_t address) {
//    if (size > 0x100) {
//        for (uint16_t i = 0; i < 0x100; i++) {
//            addressBus_[address + i] = ROM[i];
//        }
//
//        for (uint16_t i = 0x100; i < 0x800; i++) {
//            addressBus_[address + i + 0x100] = ROM[i];
//        }
//    }
//    else {
//        for (uint16_t i = 0; i < size; i++) {
//            addressBus_[address + i] = ROM[i];
//        }
//    }
//}
//
//void CPU::loadGameROM(std::string filePath) {
//    //filePath = "D:\\Games\\GBA\\Pokemon Red\\Pokemon red.gb";
//    //filePath = "D:\\Games\\GBA\\Tetris\\Tetris.gb";
//    filePath = "D:\\Games\\GBA\\dmg-acid2.gb";
//    //filePath = "D:\\Games\\GBA\\cpu_instrs.gb";
//    //filePath = "D:\\Games\\GBA\\01-special.gb";
//    //filePath = "D:\\Games\\GBA\\02-interrupts.gb";
//    //filePath = "D:\\Games\\GBA\\03-op sp,hl.gb";
//    //filePath = "D:\\Games\\GBA\\04-op r,imm.gb";
//    //filePath = "D:\\Games\\GBA\\05-op rp.gb";
//    //filePath = "D:\\Games\\GBA\\06-ld r,r.gb";
//    //filePath = "D:\\Games\\GBA\\07-jr,jp,call,ret,rst.gb";
//    //filePath = "D:\\Games\\GBA\\08-misc instrs.gb";
//    //filePath = "D:\\Games\\GBA\\09-op r,r.gb";
//    //filePath = "D:\\Games\\GBA\\10-bit ops.gb";
//    //filePath = "D:\\Games\\GBA\\11-op a,(hl).gb";
//    std::ifstream gameFile(filePath, std::ios::binary);
//    gameFile.read((char*)(addressBus_), 0x7FFF);
//    gameFile.close();
//}

void CPU::step() {
    if (!(*mmu->addrBus(0xFF00) & 0x10)) {
        *mmu->addrBus(0xFF00) |= jstate1;
    }
    else {
        *mmu->addrBus(0xFF00) |= jstate2;
    }
    
    if (clock == 0) {
        getInput();
        clock = 1000;
    }
    else {
        clock -= 1;
    }

    updateTimer();
    checkForInterupts();

    if(!isHalted_) {
        if (clock_ == 0) {
            execute(*mmu->addrBus(PC_));
        }
        clock_ -= 1;
    }
}

void CPU::getInput() {
    if (GetKeyState(VK_RIGHT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate1 &= 0xFE;
    }
    else {
        if (!(jstate1 & 0x01)) {
            *IF |= 0x10;
        }
        jstate1 |= 0x01;
    }
    if (GetKeyState(VK_LEFT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate1 &= 0xFD;
    }
    else {
        if (!(jstate1 & 0x02)) {
            *IF |= 0x10;
        }
        jstate1 |= 0x02;
    }
    if (GetKeyState(VK_UP) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate1 &= 0xFB;
    }
    else {
        if (!(jstate1 & 0x04)) {
            *IF |= 0x10;
        }
        jstate1 |= 0x04;
    }
    if (GetKeyState(VK_DOWN) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate1 &= 0xF7;
    }
    else {
        if (!(jstate1 & 0x08)) {
            *IF |= 0x10;
        }
        jstate1 |= 0x08;
    }
    
    if (GetKeyState('s') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate2 &= 0xFE;
    }
    else {
        if (!(jstate2 & 0x01)) {
            *IF |= 0x10;
        }
        jstate2 |= 0x01;
    }
    if (GetKeyState('a') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate2 &= 0xFD;
    }
    else {
        if (!(jstate2 & 0x02)) {
            *IF |= 0x10;
        }
        jstate2 |= 0x02;
    }
    if (GetKeyState(VK_RSHIFT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate2 &= 0xFB;
    }
    else {
        if (!(jstate2 & 0x04)) {
            *IF |= 0x10;
        }
        jstate2 |= 0x04;
    }
    if (GetKeyState(VK_RETURN) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        jstate2 &= 0xF7;
    }
    else {
        if (!(jstate2 & 0x08)) {
            *IF |= 0x10;
        }
        jstate2 |= 0x08;
    }
}

void CPU::execute(uint8_t instruction) {
    //debug_ = true;
    if (PC_ == 0x0100) {
        //loadGameROM("");
        mmu->BIOSMapped = false;
    }

 /*   if (PC_ == 0x02C7) {
        debug_ = true;
    }
    if (PC_ == 0x02FA) {
        debug_ = false;
    }
    if (PC_ == 0x02CA) {
        debug_ = true;
    }
    if (PC_ == 0x02E7) {
        debug_ = false;
    }*/

    uint8_t opcode = (instruction & 0b11000000) >> 6;
    uint8_t register1 = (instruction & 0b00111000) >> 3;
    uint8_t register2 = (instruction & 0b00000111);
    (this->*instructionMethods1_[instruction])(opcode, register1, register2);
    if (debug_) { printf("Instruction: 0x%x\n", instruction); printRegs(); }
    PC_ += 1;
}

void CPU::updateTimer() {
    *DIV += 1;
    if (*TAC & 0x04) {
        uint16_t sum = *TIMA;
        if (*TAC & 0x03) {
            if (*DIV & (0x0002 << ((*TAC & 0x03) * 2))) {
                fallingEdgeDelay_ = true;
            }
            else {
                if (fallingEdgeDelay_) {
                    sum += 1;
                    fallingEdgeDelay_ = false;
                }
            }
        }
        else {
            if (*DIV & 0x0200) {
                fallingEdgeDelay_ = true;
            }
            else {
                if (fallingEdgeDelay_) {
                    sum += 1;
                    fallingEdgeDelay_ = false;
                }
            }
        }

        if (sum > 0x00FF) {
            *TIMA = *TMA;
            *IF |= 0x04;
        }
        else {
            *TIMA = sum & 0x00FF;
        }
    }
}

void CPU::checkForInterupts() {
    if (*mmu->addrBus(0xFF0F)) { // Interupt Request flag
        uint8_t mask = 0x01;
        for (int i = 0; i < 5; i++) {
            if ((*mmu->addrBus(0xFF0F) & mask) && (*mmu->addrBus(0xFFFF) & mask)) {
                isHalted_ = false;

                if (IME_) {
                    IME_ = false;
                    *mmu->addrBus(0xFF0F) &= !mask;
                    *mmu->addrBus(--SP_) = (0xFF00 & PC_) >> 8;
                    *mmu->addrBus(--SP_) = 0x00FF & PC_;
                    PC_ = 0x0040 + (8 * i);
                    clock_ += 4;
                }
                break;
            }
            mask = mask << 1;
        }
    }
}

void CPU::LD_R_to_R(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        registers_[reg1] = *mmu->addrBus(getHL());
        clock_ = 8;
        if (debug_) { printf("LD %c, (HL)\n", regNames_[reg1]); }
    }
    else {
        if (reg1 == 0x06) {
            *mmu->addrBus(getHL()) = registers_[reg2];
            clock_ = 8;
            if (debug_) { printf("LD (HL), %c\n", regNames_[reg2]); }
        }
        else {
            registers_[reg1] = registers_[reg2];
            clock_ = 4;
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
                address = 0xFF00 + *mmu->addrBus(++PC_);
                if (address == 0xFF00) {
                    *mmu->addrBus(address) = registers_[A] & 0x30;
                }
                else {
                    *mmu->addrBus(address) = registers_[A];
                }
                clock_ = 12;
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + *mmu->addrBus(++PC_);
                registers_[A] = *mmu->addrBus(address);
                clock_ = 12;
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
                *mmu->addrBus(address) = registers_[A];
                clock_ = 8;
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x05:
                address = *mmu->addrBus(++PC_) + (*mmu->addrBus(++PC_) << 8);
                *mmu->addrBus(address) = registers_[A];
                clock_ = 16;
                if (debug_) { printf("LD (0x%04X), A\n", address); }
                break;
            case 0x06:
                address = 0xFF00 + registers_[C];
                registers_[A] = *mmu->addrBus(address);
                clock_ = 8;
                if (debug_) { printf("LD A, (0x%04X)\n", address); }
                break;
            case 0x07:
                address = *mmu->addrBus(++PC_) + (*mmu->addrBus(++PC_) << 8);
                registers_[A] = *mmu->addrBus(address);
                clock_ = 16;
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
                *mmu->addrBus(getBC()) = registers_[A];
                clock_ = 8;
                if (debug_) { printf("LD (BC), A\n"); }
                break;
            case 0x01:
                registers_[A] = *mmu->addrBus(getBC());
                clock_ = 8;
                if (debug_) { printf("LD A, (BC)\n"); }
                break;
            case 0x02:
                *mmu->addrBus(getDE()) = registers_[A];
                clock_ = 8;
                if (debug_) { printf("LD (DE), A\n"); }
                break;
            case 0x03:
                registers_[A] = *mmu->addrBus(getDE());
                clock_ = 8;
                if (debug_) { printf("LD A, (DE)\n"); }
                break;
            case 0x04:
                *mmu->addrBus(hl) = registers_[A];
                setHL(hl + 1);
                clock_ = 8;
                if (debug_) { printf("LD (HL+), A\n"); }
                break;
            case 0x05:
                registers_[A] = *mmu->addrBus(hl);
                setHL(hl + 1);
                clock_ = 8;
                if (debug_) { printf("LD A, (HL+)\n"); }
                break;
            case 0x06:
                *mmu->addrBus(hl) = registers_[A];
                setHL(hl - 1);
                clock_ = 8;
                if (debug_) { printf("LD (HL-), A\n"); }
                break;
            case 0x07:
                registers_[A] = *mmu->addrBus(hl);
                setHL(hl - 1);
                clock_ = 8;
                if (debug_) { printf("LD A, (HL-)\n"); }
                break;
            default:
                break;
            }
        }
        else if (reg2 == 0x06) {
            uint8_t val = *mmu->addrBus(++PC_);
            if (reg1 == 0x06) {
                *mmu->addrBus(getHL()) = val;
                clock_ = 12;
                if (debug_) { printf("LD (HL), 0x%02X\n", val); }
            }
            else {
                registers_[reg1] = val;
                clock_ = 8;
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
                lVal = *mmu->addrBus(SP_++);
                hVal = *mmu->addrBus(SP_++);
                setBC(hVal, lVal);
                clock_ = 12;
                if (debug_) { printf("POP BC\n"); }
                break;
            case 0x02:
                lVal = *mmu->addrBus(SP_++);
                hVal = *mmu->addrBus(SP_++);
                setDE(hVal, lVal);
                clock_ = 12;
                if (debug_) { printf("POP DE\n"); }
                break;
            case 0x04:
                lVal = *mmu->addrBus(SP_++);
                hVal = *mmu->addrBus(SP_++);
                setHL(hVal, lVal);
                clock_ = 12;
                if (debug_) { printf("POP HL\n"); }
                break;
            case 0x06:
                lVal = *mmu->addrBus(SP_++);
                hVal = *mmu->addrBus(SP_++);
                setAF(hVal, lVal);
                clock_ = 12;
                if (debug_) { printf("POP AF\n"); }
                break;
            case 0x07:
                SP_ = getHL();
                clock_ = 8;
                if (debug_) { printf("LD SP, HL\n"); }
                break;
            default:
                break;
            }
        }
        else {
            int8_t val;
            switch (reg1)
            {
            case 0x00:
                *mmu->addrBus(--SP_) = registers_[B];
                *mmu->addrBus(--SP_) = registers_[C];
                clock_ = 16;
                if (debug_) { printf("PUSH BC\n"); }
                break;
            case 0x02:
                *mmu->addrBus(--SP_) = registers_[D];
                *mmu->addrBus(--SP_) = registers_[E];
                clock_ = 16;
                if (debug_) { printf("PUSH DE\n"); }
                break;
            case 0x04:
                *mmu->addrBus(--SP_) = registers_[H];
                *mmu->addrBus(--SP_) = registers_[L];
                clock_ = 16;
                if (debug_) { printf("PUSH HL\n"); }
                break;
            case 0x06:
                *mmu->addrBus(--SP_) = registers_[A];
                *mmu->addrBus(--SP_) = registers_[F];
                clock_ = 16;
                if (debug_) { printf("PUSH AF\n"); }
                break;
            case 0x07:
                val = *mmu->addrBus(++PC_);
                setHL(SP_ + val);

                // Set Z flag to 0
                setZ(false);
                // Set N flag to 0
                setN(false);
                // Calculate if Half-Carry flag needs to be set
                setH(((SP_ ^ getHL() ^ val) & 0x0010) == 0x0010);
                // Calculate if Carry flag needs to be set
                setC(((SP_ ^ getHL() ^ val) & 0x0100) == 0x0100);

                clock_ = 12;
                if (debug_) { printf("HL, SP +%d\n", val); }
                break;
            default:
                break;
            }
        }
    }
    else {
        uint8_t lVal = *mmu->addrBus(++PC_);
        uint8_t hVal = *mmu->addrBus(++PC_);
        uint16_t addr;
        switch (reg1)
        {
        case 0x00:
            setBC(hVal, lVal);
            clock_ = 12;
            if (debug_) { printf("LD BC, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x01:
            addr = (hVal << 8) + lVal;
            *mmu->addrBus(addr) = getSP() & 0x00FF;
            *mmu->addrBus(addr + 1) = (getSP() & 0xFF00) >> 8;
            clock_ = 20;
            if (debug_) { printf("LD (0x%04X), SP\n", addr); }
            break;
        case 0x02:
            setDE(hVal, lVal);
            clock_ = 12;
            if (debug_) { printf("LD DE, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x04:
            setHL(hVal, lVal);
            clock_ = 12;
            if (debug_) { printf("LD HL, 0x%02X%02X\n", hVal, lVal); }
            break;
        case 0x06:
            SP_ = (hVal << 8) + lVal;
            clock_ = 12;
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
        clock_ = 4;
        if (debug_) { printf("JP HL\n"); }
    }
    else {
        uint8_t lVal = *mmu->addrBus(++PC_);
        uint8_t hVal = *mmu->addrBus(++PC_);
        uint16_t addr = (hVal << 8) + lVal;
        clock_ = 12;

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
        clock_ = 16;
    }
}

void CPU::JR(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    int8_t nVal = *mmu->addrBus(++PC_);
    clock_ = 8;

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
    clock_ = 12;
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
        if (debug_) { printf("ADD "); }
    }

    // Get the value being used for the calculation with Register A
    if (op == 0x03) {
        nVal = *mmu->addrBus(++PC_);
        clock_ = 8;
        if (debug_) { printf("A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            clock_ = 4;
            if (debug_) { printf("A, %c\n", regNames_[reg2]); }
        }
    }

    // Perform addition to A register
    registers_[A] += nVal + carry;
    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set N flag to 0
    setN(false);
    // Calculate if Half-Carry flag needs to be set
    setH(((rVal & 0x0F) + (nVal & 0x0F) + carry) > 0x0F);
    // Calculate if Full-Carry flag needs to be set
    setC((rVal + nVal + carry) > 0xFF);
}

void CPU::SUB(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t rVal = registers_[A];
    uint8_t nVal;
    bool carry = false;

    // Check if carry bit will be used
    if (reg1 == 0x03) {
        carry = getC();
        if (debug_) { printf("SBC "); }
    }
    else {
        if (debug_) { printf("SUB "); }
    }

    // Get the value being used for the calculation with Register A
    if (op == 0x03) {
        nVal = *mmu->addrBus(++PC_);
        clock_ = 8;
        if (debug_) { printf("A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            clock_ = 4;
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
        uint8_t nVal = *mmu->addrBus(++PC_);
        registers_[A] &= nVal;
        clock_ = 8;
        if (debug_) { printf("AND A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] &= *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("AND A, (HL)\n"); }
        }
        else {
            registers_[A] &= registers_[reg2];
            clock_ = 4;
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
        uint8_t nVal = *mmu->addrBus(++PC_);
        registers_[A] ^= nVal;
        clock_ = 8;
        if (debug_) { printf("XOR A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] ^= *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("XOR A, (HL)\n"); }
        }
        else {
            registers_[A] ^= registers_[reg2];
            clock_ = 4;
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
        uint8_t nVal = *mmu->addrBus(++PC_);
        registers_[A] |= nVal;
        clock_ = 8;
        if (debug_) { printf("OR A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            registers_[A] |= *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("OR A, (HL)\n"); }
        }
        else {
            registers_[A] |= registers_[reg2];
            clock_ = 4;
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
        nVal = *mmu->addrBus(++PC_);
        clock_ = 8;
        if (debug_) { printf("CP A, 0x%02X\n", nVal); }
    }
    else {
        if (reg2 == 0x06) {
            nVal = *mmu->addrBus(getHL());
            clock_ = 8;
            if (debug_) { printf("CP A, (HL)\n"); }
        }
        else {
            nVal = registers_[reg2];
            clock_ = 4;
            if (debug_) { printf("CP A, %c\n", regNames_[reg2]); }
        }
    }

    // Calculate if Half-Carry flag needs to be set
    setH(((rVal - nVal) & 0x0F) > (rVal & 0x0F));
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
        *mmu->addrBus(getHL()) += 1;
        result = *mmu->addrBus(getHL());
        clock_ = 12;
        if (debug_) { printf("INC (HL)\n"); }
    }
    else {
        registers_[reg1] += 1;
        result = registers_[reg1];
        clock_ = 4;
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
        *mmu->addrBus(getHL()) -= 1;
        result = *mmu->addrBus(getHL());
        clock_ = 12;
        if (debug_) { printf("DEC (HL)\n"); }
    }
    else {
        registers_[reg1] -= 1;
        result = registers_[reg1];
        clock_ = 4;
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
        int8_t nVal = *mmu->addrBus(++PC_); // Interpreted as signed value
        uint16_t rVal = SP_;
        SP_ += nVal;

        // Set Z flag to 0
        setZ(false);
        // Set N flag to 0
        setN(false);
        // Calculate if Half-Carry flag needs to be set
        setH(((SP_ ^ rVal ^ nVal) & 0x0010) == 0x0010);
        // Calculate if Carry flag needs to be set
        setC(((SP_ ^ rVal ^ nVal) & 0x0100) == 0x0100);
        
        clock_ = 16;
        if (debug_) { printf("ADD SP, %d\n", nVal); }
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
            setHL(getHL() + SP_);
            if (debug_) { printf("ADD HL, SP\n"); }
            break;
        default:
            break;
        }

        // Set N flag to 0
        setN(false);
        // Calculate if Half-Carry flag needs to be set
        setH((getHL() & 0x0FFF) < (rVal & 0x0FFF));
        // Calculate if Carry flag needs to be set
        setC(getHL() < rVal);
        
        clock_ = 8;
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
    clock_ = 8;
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
    clock_ = 8;
}

void CPU::CBPrefix(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (debug_) { printf("CB Prefix\n"); }

    uint8_t instruction = *mmu->addrBus(++PC_);
    uint8_t opcode = (instruction & 0b11000000) >> 6;
    uint8_t register1 = (instruction & 0b00111000) >> 3;
    uint8_t register2 = (instruction & 0b00000111);
    cbPrefx_ = true;
    (this->*instructionMethods2_[instruction])(opcode, register1, register2);
    cbPrefx_ = false;
}

void CPU::RLC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        *mmu->addrBus(getHL()) = (rVal << 1) + getC();

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
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
            clock_ = 8;
            if (debug_) { printf("RLC %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            clock_ = 4;
            if (debug_) { printf("RLCA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RRC(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        *mmu->addrBus(getHL()) = (rVal >> 1) + (getC() << 7);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
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
            clock_ = 8;
            if (debug_) { printf("RRC %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            clock_ = 4;
            if (debug_) { printf("RRCA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        *mmu->addrBus(getHL()) = (rVal << 1) + getC();
        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
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
            clock_ = 8;
            if (debug_) { printf("RL %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            clock_ = 4;
            if (debug_) { printf("RLA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::RR(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        *mmu->addrBus(getHL()) = (rVal >> 1) + (getC() << 7);
        // Calculate if Carry flag needs to be set
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
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
            clock_ = 8;
            if (debug_) { printf("RR %c\n", regNames_[reg2]); }
        }
        else {
            setZ(false);
            clock_ = 4;
            if (debug_) { printf("RRA\n"); }
        }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SLA(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        *mmu->addrBus(getHL()) = rVal << 1;
        
        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
        if (debug_) { printf("SLA (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        registers_[reg2] = rVal << 1;

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        clock_ = 8;
        if (debug_) { printf("SLA %c\n", regNames_[reg2]); }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SRA(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        // Calculate if Carry flag needs to be set
        setC(rVal >= 0b10000000);
        *mmu->addrBus(getHL()) = (rVal >> 1) + (getC() << 7);
        setC(rVal % 2);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
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
        clock_ = 8;
        if (debug_) { printf("SRA %c\n", regNames_[reg2]); }
    }

    // Set H and N flags to 0
    setH(false);
    setN(false);
}

void CPU::SWAP(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());
        *mmu->addrBus(getHL()) = (rVal << 4) + (rVal >> 4);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
        if (debug_) { printf("SWAP (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];
        registers_[reg2] = (rVal << 4) + (rVal >> 4);

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        clock_ = 8;
        if (debug_) { printf("SWAP %c\n", regNames_[reg2]); }
    }

    // Set C, H and N flags to 0
    setC(false);
    setH(false);
    setN(false);
}

void CPU::SRL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x06) {
        uint8_t rVal = *mmu->addrBus(getHL());

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        *mmu->addrBus(getHL()) = (rVal >> 1);

        // Calculate if Zero flag needs to be set
        setZ(*mmu->addrBus(getHL()) == 0x00);
        clock_ = 16;
        if (debug_) { printf("SRA (HL)\n"); }
    }
    else {
        uint8_t rVal = registers_[reg2];

        // Calculate if Carry flag needs to be set
        setC(rVal % 2);
        registers_[reg2] = (rVal >> 1);

        // Calculate if Zero flag needs to be set
        setZ(registers_[reg2] == 0x00);
        clock_ = 8;
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
        setZ((*mmu->addrBus(getHL()) & mask) == 0);
        clock_ = 12;
        if (debug_) { printf("BIT %d, (HL)\n", reg1); }
    }
    else {
        // Calculate if Zero flag needs to be set
        setZ((registers_[reg2] & mask) == 0);
        clock_ = 8;
        if (debug_) { printf("BIT %d, %c\n", reg1, regNames_[reg2]); }
    }

    // Set H flag to 1, N flag to 0
    setH(true);
    setN(false);
}

void CPU::RES(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t mask = (0x01 << reg1) ^ 0xFF;

    if (reg2 == 0x06) {
        *mmu->addrBus(getHL()) &= mask;
        clock_ = 16;
        if (debug_) { printf("RES %d, (HL)\n", reg1); }
    }
    else {
        registers_[reg2] &= mask;
        clock_ = 8;
        if (debug_) { printf("RES %d, %c\n", reg1, regNames_[reg2]); }
    }
}

void CPU::SET(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t mask = 0x01 << reg1;

    if (reg2 == 0x06) {
        *mmu->addrBus(getHL()) |= mask;
        clock_ = 16;
        if (debug_) { printf("SET %d, (HL)\n", reg1); }
    }
    else {
        registers_[reg2] |= mask;
        clock_ = 8;
        if (debug_) { printf("SET %d, %c\n", reg1, regNames_[reg2]); }
    }
}

void CPU::CALL(uint8_t op, uint8_t reg1, uint8_t reg2) {
    uint8_t lAddr = *mmu->addrBus(++PC_);
    uint8_t hAddr = *mmu->addrBus(++PC_);
    clock_ = 12;

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

    PC_ += 1;
    *mmu->addrBus(--SP_) = (0xFF00 & PC_) >> 8;
    *mmu->addrBus(--SP_) = 0x00FF & PC_;
    PC_ = (hAddr << 8) + lAddr - 1;
    clock_ = 24;
}

void CPU::RET(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    if (reg2 == 0x00) {
        clock_ = 8;
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
        clock_ = 20;
    }
    else {
        if (reg1 == 0x03) {
            IME_ = true;
            if (debug_) { printf("RETI\n"); }
        }
        else {
            if (debug_) { printf("RET\n"); }
        }
        clock_ = 16;
    }

    uint8_t lAddr = *mmu->addrBus(SP_++);
    uint8_t hAddr = *mmu->addrBus(SP_++);

    PC_ = (hAddr << 8) + lAddr - 1;
}

void CPU::RST(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    PC_ += 1;
    *mmu->addrBus(--SP_) = (0xFF00 & PC_) >> 8;
    *mmu->addrBus(--SP_) = 0x00FF & PC_;
    PC_ = (reg1 * 8) - 1;

    clock_ = 16;
    if (debug_) { printf("RST\n"); }
}

void CPU::DAA(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    if (!getN()) {
        if (getC() || registers_[A] > 0x99) {
            registers_[A] += 0x60;
            setC(true);
        }
        if (getH() || (registers_[A] & 0x0F) > 0x09) {
            registers_[A] += 0x06;
        }
    }
    else if (getC()) {
        if (getH()) {
            registers_[A] += 0x9A;
        }
        else {
            registers_[A] += 0xA0;
        }
    }
    else if (getH()) {
        registers_[A] += 0xFA;
    }

    // Calculate if Zero flag needs to be set
    setZ(registers_[A] == 0);
    // Set Half-Carry flag to 0
    setH(false);

    clock_ = 4;
    if (debug_) { printf("DAA\n"); }
}

void CPU::CPL(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    registers_[A] = 0xFF - registers_[A];

    // Set N flag to 1
    setN(true);
    // Set H flag to 1
    setH(true);

    clock_ = 4;
    if (debug_) { printf("CPL\n"); }
}

void CPU::SCF(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    // Set N flag to 0
    setN(false);
    // Set H flag to 0
    setH(false);
    // Set C flag to 1
    setC(true);

    clock_ = 4;
    if (debug_) { printf("SCF\n"); }
}

void CPU::CCF(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    // Set N flag to 0
    setN(false);
    // Set H flag to 0
    setH(false);
    // Set C flag to !C
    setC(!getC());
    clock_ = 4;
    if (debug_) { printf("CCF\n"); }
}

void CPU::DI(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    IME_ = false;
    clock_ = 4;
    if (debug_) { printf("DI\n"); }
}

void CPU::EI(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    IME_ = true;
    clock_ = 4;
    if (debug_) { printf("EI\n"); }
}

void CPU::HALT(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    isHalted_ = true;
    clock_ = 4;
    if (debug_) { printf("HALT\n"); }
}

void CPU::STOP(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    // IME = true;
    clock_ = 4;
    if (debug_) { printf("STOP\n"); }
}

void CPU::nop(uint8_t instruction, uint8_t reg1, uint8_t reg2) {
    instruction = (instruction << 6) + (reg1 << 3) + reg2;
    clock_ = 4;
    if (debug_) { printf("NOP 0x%02X\n", instruction); }
}

void CPU::printRegs() {
    printf("REGS: \nA: 0x%02X F: 0x%02X\nB: 0x%02X C: 0x%02X\nD: 0x%02X E: 0x%02X\nH: 0x%02X L: 0x%02X\nZ: %d N: %d H: %d C: %d\nPC: 0x%04X\nSP: 0x%04X\n\n", 
        registers_[A], registers_[F], registers_[B], registers_[C],
        registers_[D], registers_[E], registers_[H], registers_[L],
        getZ(), getN(), getH(), getC(),
        PC_ + 1, SP_);
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
    registers_[F] = lVal & 0xF0;
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
