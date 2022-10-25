#include "timer.h"

const int Timer::DIV_BIT[] = {7,1,3,5};

Timer::Timer(Machine* machine) {
    this->machine = machine;
}

void Timer::increment(int cycles) {
    for (int i = 0; i < cycles; i++) {
        if (this->pendingOverflow) {
            this->tima = this->tma;
            //this->machine->interruptsFired |= 4;
            this->pendingOverflow = false;
        }
        this->divider = (this->divider + 1) & 0xffff;
        this->updateEdge();
    }
}

void Timer::updateEdge() {
    bool bit = (this->divider & (1 << DIV_BIT[this->tac & 3])) != 0;
    bit &= (this->tac & 4) != 0;
    if (this->delayed && !bit) {
        this->tima += 1;
        if (this->tima > 0xff) {
            this->pendingOverflow = true;
            this->tima = 0;
        }
    }
    this->delayed = bit;
}

int Timer::read(int address) {
    switch (address) { // Already &3'd
    case 0:
        return (this->divider >> 6) & 0xff;
    case 1:
        return this->tima;
    case 2:
        return this->tma;
    case 3:
        return this->tac | 0xF8;
    }
    return 0;
}

void Timer::write(int address, int value) {
    switch (address) { // ibid
    case 0:
        this->divider = 0; break;
    case 1:
        this->pendingOverflow = false;
        this->tima = value; break;
    case 2:
        this->tma = value; break;
    case 3:
        value &= 7;
        this->tac = value; break;
    }
    this->updateEdge();
}

void Timer::save(std::ostream dos) {
    //dos.write("TIME".getBytes(StandardCharsets.UTF_8));
    //dos.writeInt(divider);
    //dos.writeInt(tima);
    //dos.writeInt(tma);
    //dos.writeInt(tac);
    //dos.writeBoolean(delayed);
    //dos.writeBoolean(pendingOverflow);
}

void Timer::load(std::istream dis) {
    //divider = dis.readInt();
    //tima = dis.readInt();
    //tma = dis.readInt();
    //tac = dis.readInt();
    //delayed = dis.readBoolean();
    //pendingOverflow = dis.readBoolean();
}

void Timer::printDebugState() {
    std::cout << "DIV: " << this->divider 
              << "TIMA: " << this->tima
              << "TMA: " << this->tma
              << "TAC: " << this->tac << std::endl;
    std::cout << "Last latch? " << this->delayed 
              << ", overflow processing? " << this->pendingOverflow << std::endl;
}
