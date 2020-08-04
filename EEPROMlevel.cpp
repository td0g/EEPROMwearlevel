

#include "EEPROMlevel.h"
#include <EEPROM.h>

EEPROMlevel::EEPROMlevel(uint8_t data[], uint8_t blockSize, uint8_t checkSize,uint16_t iStart, uint16_t iEnd)
{
  //Sanity checks
  _indexSize = min(checkSize, 4);                                 //EEPROM won't last that long... 4 bytes max!
  _data = data;                                                   //This is the pointer to the array
  _start = min(iStart+_indexSize, EEPROM.length() - _blockSize);  //Cannot start past last usable index
  iEnd = min(iEnd, EEPROM.length() - 1);                          //Cannot end past last usable index
  _blockSize = min(blockSize, iEnd - _start);                     //block size cannot exceed given space
  _blockCount = (iEnd - iStart) / _blockSize - 1;
}

void EEPROMlevel::load(int16_t age){
  //Load EEPROM data to array
  if (abs(age) > _blockCount){    //Cannot select entry older than the number of blocks!
    if (age > 0) age = _blockCount;
    else age = _blockCount * -1;
  }
  int16_t currBlock = readHeadLocation() + age * _blockSize;
  if (currBlock < _start) currBlock = currBlock + _blockSize * _blockCount;
  for (int i = 0; i < _blockSize; i++){
    _data[i] = EEPROM.read(i + currBlock);
    if (!i) _data[i] &= 0b01111111; //Mask out the check bit
  }
}

void EEPROMlevel::save(){
	uint16_t currBlock = writeHeadLocation();
	uint8_t checkBit = 0b10000000 - (EEPROM.read(currBlock) & 0b10000000);	//Is the check bit set?
	for (int i = 0; i < _blockSize; i++){
		if (!i) {
		  EEPROM.update(i + currBlock, (_data[i] & 0b01111111) | checkBit);	//If this is the check byte, set/clear the check bit
      if (currBlock + _blockSize > _start + _blockSize * _blockCount){
        if ((EEPROM.read(currBlock + _blockSize) & 0b10000000) == checkBit) EEPROM.write(currBlock + _blockSize, 128 - checkBit);
      }
		}
		else EEPROM.update(i + currBlock, _data[i]);	//Update the byte, repeart for the entire block
	}
  if (_indexSize && writeHeadLocation() == _start){
    uint32_t index = getLastIndex() + 1;
    for (int i = 0; i < _indexSize; i++){
      EEPROM.update(_start - _indexSize + i, index); 
      index >>= 8;
    }
  }
}

void EEPROMlevel::update(){
  uint16_t currBlock = readHeadLocation();
	uint8_t checkBit = EEPROM.read(readHeadLocation()) & 0b10000000;	//Is the check bit set?
	for (int i = 0; i < _blockSize; i++){
    uint8_t mask = 0b11111111;
    if (!i && !_indexSize) mask = 0b01111111;
    if ((EEPROM.read(currBlock + i) & mask) != (_data[i] & mask)){
      save();
      return;
    }
	}
}


uint16_t EEPROMlevel::writeHeadLocation(){
  uint8_t a = EEPROM.read(_start) & 0b10000000;	//Is the check bit set for the first block? 
  for (int i = 1; i < _blockCount; i++){
	  if ((EEPROM.read(_start + i * _blockSize) & 0b10000000) != a) return _start + i * _blockSize;	//If the check bit has changed since the last block, return the first block index
  }
  return _start; //All check bits match, so return the first block index
}

uint16_t EEPROMlevel::readHeadLocation(){
	uint16_t a = writeHeadLocation();
	if (a == _start) return _start + _blockSize * (_blockCount - 1);	//If saving to first block, then load from the last block
	return a - _blockSize;	//Otherwise load from the block immediately prior to the saving block
}


uint32_t EEPROMlevel::getLastIndex(){
  uint32_t index = 0;
  for (byte i = 0; i < _indexSize; i++){
    index <<= 8;
    index |= EEPROM.read(_start - 1 - i);
  }
  return index;
}

uint32_t EEPROMlevel::getTotalEntryCount(){
  uint32_t index = getLastIndex();
  if (writeHeadLocation() == _start && index) index--;
  return _blockCount * index + (readHeadLocation() - _start) / _blockSize;
}

uint16_t EEPROMlevel::getTotalBlocks(){
  return _blockCount;
}
