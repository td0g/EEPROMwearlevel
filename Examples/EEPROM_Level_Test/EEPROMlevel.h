/*
Issues:
  If EEPROM isn't cleared, issues may occur when finding last entry

Function:
  The library takes an allocated area of EEPROM (the entire memory by default) and divides it into BLOCKS
  Each block contains DATA and a CHECK INDEX
  The first byte in the DATA array can only hold 7 bits of data.  The MSB is reserved and masked by the library (not shown and not changeable).
  The CHECK INDEX size is 0 - 4 bytes
  A 0 byte size is useful for when the total number of data entries isn't required (eg. saving settings for a program)
  1 - 4 bytes are useful for when the total number of data entries is important (eg. counting the number of times a device has performed an operation)
    The maximum value of the data entry counter = 256 ^ [# OF BYTES] * [EEPROM ALLOCATION SIZE] / [BLOCK SIZE] - 1
    So 2 bytes for the CHECK INDEX and 2 bytes for data across the entire EEPROM of an Arduino Uno (1K EEPROM) results a maximum counter value of 16,777,215
    The totalEntryCount() returns the counter value.

  Data is loaded from the EEPROM to an array using the load() function
    To load the currently stored data, do not use any parameters
    To load older data, use the age parameter: load(age)
    The age is the number of entries to go back.
      Age = 0 will access the most recent entry, 
      Age = -1 will access the first prior entry
      Age = -2 will access the second prior entry
      Age = 1 will access the oldest entry
      Age = 2 will access the second oldest entry
  Data is saved to the EEPROM using the save() or update() function
    The save() function will always enter the data into a new block
    The update() function will only enter the data if it doesn't match the last entry
    Using the update() function frequently is safe, but the save() function will cause EEPROM wear
    Using the save() function will always increment the data entry counter, but the update() function will only update it if the data has changed
 
0.1
  2020-07-28
  Functional
0.2
  2020-07-29
  Can now use 1 - 4 bytes of EEPROM per block to store total entry count
0.3
  2020-07-29
  Debugged and tested.  See testing schedule, millis per operation recorded
0.4
  2020-08-01
  Replaced readOldData() with new load() parameter
0.5
  2020-08-01
  Restructured index.  MSB of first byte is ALWAYS reserved and masked.
  The index count bytes are located before the data arrays.  This will only be updated once per cycle through all the blocks, so no additional wear.
  This is more efficient use of space
1.0
  2020-08-04
  Initial release version
  Improved documentation, included examples
 */

#ifndef EEPROMLEVEL_H
#define EEPROMLEVEL_H
#include <Arduino.h>
#include <EEPROM.h>

class EEPROMlevel {

  public:
/*
Constructor
set data array, size of block (in bytes), number of bytes to record total entry count, start index of wear levelling area, set end index of wear levelling area
*/    
    EEPROMlevel(uint8_t data[], uint8_t blockSize = 1, uint8_t checkSize = 0, uint16_t iStart = 0, uint16_t iEnd = EEPROM.length());

//Load the array from EEPROM to the data array
//PARAMETERS: 
//  age - 0 will load the currently stored data
//  -1 will load the first prior data, -2 will load the second prior data, etc.
//  +1 will load the earlest data, +2 will load the second earliest data, etc.
    void load(int16_t age = 0);
  
//Save an array of bytes.  This always increments the total count and will cause wear to the EEPROM each time it is used.
	  void save();
	
//Save an array of bytes ONLY if it isn't identical to last data set.  This does not increment the total count unless a save operation is performed.
//This function can be called frequently without wearing the EEPROM, unless the data is also updated frequently.
	  void update();

//Get a total count for the number of times data was saved
//If 0 bytes are used for the index, then this function will always return 0
//Otherwise, this function will return the total number of save operations performed.
    uint32_t getTotalEntryCount();

//Get the total number of blocks
    uint16_t getTotalBlocks();

  private:

//Returns the EEPROM address of the first byte in the next-to-be-written block
    uint16_t writeHeadLocation();
    
//Returns the EEPROM address of the first byte in the most recently written block
    uint16_t readHeadLocation();

//Returns the stored index value
//The number of bytes stored for the index is set during initialization (0 - 4)
    uint32_t getLastIndex();
    
//Pointer to the data array
    uint8_t* _data;

//Size of each block
    uint8_t _blockSize;

//Size of the indexx
    uint16_t _indexSize;

//First data address in EEPROM
//The index is stored immediately prior to this
    int16_t _start;

//Number of usable blocks
    uint16_t _blockCount;
};


#endif
