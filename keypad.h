#pragma once

#include <iostream>
#include "machine.h"

class Machine;

class Keypad {
public:
    enum Key {
        KEY_RIGHT,
        KEY_LEFT,
        KEY_UP,
        KEY_DOWN,
        KEY_A,
        KEY_B,
        KEY_SELECT,
        KEY_START
    };

    /*
     * @param machine Parent machine
     */
    Keypad(Machine* );

    /*
     * Write the register value
     * @param val Value being written
     */
    void write(int );

    /*
     * @return Current keypad value
     */
    int read();

    /*
     * Processed when a key goes down
     * @param key Key code
     */
    void keyDown(Key );

    /*
     * Processed when a key is released
     * @param key Key code
     */
    void keyUp(Key );

    /*
     * Save key state
     * @param dos Output destination stream
     * @throws IOException From inner writes
     */
    void save(std::ostream );

    /*
     * Load state
     * @param dis Source stream
     * @throws IOException From inner reads
     */
    void load(std::istream );

private:
    int keysUp = 0xff;
    bool p14, p15;
    Machine* machine;
};
