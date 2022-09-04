#include "cpu.h"

CPU::CPU(Machine* machine) {
    this->machine = machine;
    this->registers = new Registers();
}