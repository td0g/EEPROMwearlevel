# EEPROMwearlevel

EEPROM has a limited number of write cycles, usually rated around 100,000.  If the same EEPROM is used frequently to store data, it will eventually fail.  However, if the data is stored on a different area of the EEPROM each time, then the EEPROM will wear more evenly and many more data save cycles can be performed before failure.

## How It Works

This library works by saving data to a different EEPROM address each time.  So instead of updating address 0 each time, it will save to address 0, then 1, next 2, etc.  When it comes to the end of the EEPROM memory, then it will loop back to the first address

Because the advantage of EEPROM is that it is non-volatile, the library must be able to determine the last address where data was saved on boot-up.  This is done with a 'check bit', which is the Most Significant Bit (MSB) of the first byte in each block (set of bytes which are updated during data saving).  When data is saved to a block, the check bit for that block is flipped.

To find the most recent data, the library examines the check bit of the first block and begins searching for the last block with a matching check bit.  If all the check bits match, then the very last block was the most recently saved block.

## What Is It Useful For

### Storing Settings

Settings which are frequently updated can be saved more often without danger of EEPROM failure.  Using the update() function is particularly useful since the data will only be saved if they have been changed.

### Datalogging

Data can be more frequently written to EEPROM thanks to the wear levelling algorithm.  Not only that, but previous data can also be accessed!  

### Odometer

A small number (1 - 4) of bytes can be used to keep a tally of the total write cycles.  These bytes recieve the same amount of wear as the data bytes.

## License

Software is licensed under a GNU GPL v3 License
