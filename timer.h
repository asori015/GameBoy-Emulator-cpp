#pragma once

#include <iostream>
#include "machine.h"

class Machine;

class Timer {
public:
    // NOTE: Allegedly, the divider is actually in m-cycles, not t-cycles,
    // hence why the bit counts are off-by-2

    /*
     * @param machine Parent machine
     */
    Timer(Machine* );

    /*
     * Advance the timer by cycles m-cycles
     * @param cycles m-cycles to advance
     */
    void increment(int );

    /*
     * Trigger an interrupt if the right bits are flipped
     */
    void updateEdge();

    /*
     * @param address Address to read
     * @return Read value
     */
    int read(int );

    /*
     * @param address Address to write
     * @param value Value to write
     */
    void write(int , int );

    /*
     * Save Timer state
     * @param dos Dest stream
     * @throws IOException Errors writing
     */
    void save(std::ostream );

    /**
     * Load timer state
     * @param dis Source stream
     * @throws IOException Errors reading
     */
    void load(std::istream );

    /**
     * Print debug state of timer
     */
    void printDebugState();
    
    int divider;
private:
    int tima;
    int tma;
    int tac;
    bool delayed;
    bool pendingOverflow;
    Machine* machine;
    static const int DIV_BIT[];
};
