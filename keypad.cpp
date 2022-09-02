#include "keypad.h"

Keypad::Keypad(Machine* machine) {
    this->machine = machine;
}

void Keypad::write(int val) {
    this->p14 = (val & 0x10) == 0;
    this->p15 = (val & 0x20) == 0;
}

int Keypad::read() {
    int keys = 0;
    if (this->p14)
        keys |= this->keysUp & 0xf;
    if (this->p15)
        keys |= this->keysUp >> 4;
    return keys;
}

void Keypad::keyDown(Key key) {
    if ((this->keysUp & (1 << key)) != 0) {
        this->machine->interruptsFired |= 0x10;
        this->machine->stop = false;
    }
    this->keysUp &= ~(1 << key);
}

void Keypad::keyUp(Key key) {
    this->keysUp |= (1 << key);
}

void Keypad::save(std::ostream dos) {
    //dos.write("JOYP".getBytes(StandardCharsets.UTF_8));
    //dos.writeInt(this->keysUp);
    //dos.writeBoolean(this->p14);
    //dos.writeBoolean(this->p15);
}

void Keypad::load(std::istream dis) {
   /* this->keysUp = dis.readInt();
    this->p14 = dis.readBoolean();
    this->p15 = dis.readBoolean();*/
}
