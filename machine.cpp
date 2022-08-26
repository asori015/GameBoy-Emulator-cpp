#include <iostream>
#include "machine.h"

Machine::Machine(std::string msg){
    this->msg = msg;
}

void Machine::hello(){
    std::cout << this->msg << std::endl;
}