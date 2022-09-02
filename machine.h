#pragma once

#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "timer.h"
#include "keypad.h"
#include "soundboard.h"

class Machine{
    public:
     class ROMException : public std::exception {
         public:
          std::string what() {
              return "ROM Exception";
          }
     };

     struct MachineMode {
         int afInitial;
         bool isCgb;
         char* BIOS;
     };

     enum class MachineModeEnum {
         GAMEBOY,
         GAMEBOY_POCKET,
         GAMEBOY_COLOR
     };

     /*
     * Create a machine with a loaded ROM
     * @param ROM ROM file
     * @param mode Machine mode to boot into
     */
     Machine(std::string, MachineModeEnum );

     /*
     * Create a machine with a loaded ROM
     * @param ROM ROM file
     * @param mode Machine mode to boot into
     * @param saveFile File to save external RAM to (or NULL)
     */
     Machine(std::string, MachineModeEnum, std::string);

     /*
     * @return True if switch speed succeeded. False otherwise.
     */
     bool trySpeedSwitch();

     /*
     * Perform one instruction cycle
     */
     void cycle();

     /*
     * Used for debugging, prints state info 
     */
     void printDebugState();

     /*
     * Test single opcode tests
     * @param source Inputstream to file specifying test
     */
     void test();

     /*
     * @param screen Screen to attach to the GPU
     */
     void attachScreen( );

     /*
     * @param speaker Speaker to attach to the SoundBoard
     */
     void attachSpeaker();

     /*
     * Sets the mute status of the SoundBoard
     * @param muted True if the SoundBoard should be muted
     */
     void mute(bool );

     /*
     * Save the machine state
     * @param os Destination stream
     * @throws RomException Error writing state
     */
     void saveState( );

     /*
     * Load machine state
     * @param is Source stream
     * @throws RomException Error reading state
     */
     void loadState();

     /*
     * Save external RAM (e.g. save file)
     * @param os Destination stream
     * @throws RomException Error writing save
     */
     void saveExternal();

     /*
     * Load external RAM (e.g. save file)
     * @param is Source stream
     * @throws RomException Error reading save
     */
     void loadExternal();

     /*
     * Save state to default save file
     * @throws RomException Errors writing save
     */
     void saveExternal(int );

     /*
     * Loads save from default file
     * @throws RomException Errors reading save
     */
     void loadExternal(int );

     // GETTERS AND SETTERS

     /*
     * @return A base path that can be used for save states, based on the
     * path of the ROM loaded
     */
     std::string getBaseNamePath() { return this->baseNamePath; }

     /*
     * @return Exposes the MMU, used for cheats
     */
     MMU getMmu() { return this->mmu; }

     /*
     * @return The attached keypad
     */
     Keypad getKeypad() { return this->keypad; }

     /*
     * Exposed so the frontend can adjust palettes
     * @return The palette used when NOT in color mode
     */
     int* getDmgPallet() { return this->gpu.greyPallete; }

     /*
     * Exposed so the frontend can adjust palettes
     * @return The color palette used for background in CGB mode
     */
     int* getCgbBgPalette() { return this->gpu.bgPalColor; }

     /*
     * Exposed so the frontend can adjust palettes
     * @return The color palette used for sprites in CGB mode
     */
     int* getCgbObPalette() { return this->gpu.obPalColor; }

     CPU cpu;
     MMU mmu;
     GPU gpu;
     Timer timer;
     Keypad keypad;
     SoundBoard soundBoard;

     bool halt;
     bool stop;

     int interruptsEnabled;
     int interruptsFired;

     /*
     * Speeds up emulation by specified factor.
     * Do not set to 0!
     */
     int speedUp = 1;

     MachineMode mode;

     std::string saveFilePath;

     // For CGB
     bool doubleSpeed;

    private:
     std::string saveExtension(std::string );

     // For CGB
     bool usingColor;
     bool monochromeCompatabiliy;
     std::string baseNamePath;
};
