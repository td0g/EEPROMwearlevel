/*
  Example firmware for EEPROMlevel library
  Written by Tyler Gerritsen
  vtgerritsen@gmail.com
  www.td0g.ca

DESCRIPTION:
  This firmware records the adc values from pins A0 and A1 every minute
  To read the recorded data, send a character over serial

LICENSE:
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

CHANGELOG
  2020-08-04
    1.0
    Initial Release
 */

#include "EEPROMlevel.h"
#define ARRAY_SIZE 4
uint8_t data[ARRAY_SIZE];
EEPROMlevel mem(data, ARRAY_SIZE, 4);

unsigned long timer;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (millis() > timer){
    timer += 60000; //Do this every minute

//Read from A0 and A1, then store to the data array
    unsigned int sig = analogRead(A0);
    data[1] = sig;
    sig >>= 8;
    data[0] = sig;  //Only 2 bits are used (ADC is 10-bit), perfect since only 7 bits are available here
    sig = analogRead(A1);
    data[3] = sig;
    sig >>= 8;
    data[2] = sig;
    mem.save();
  }

//If serial character is recieved, then send the entire available history
  if (Serial.available()){
    while (Serial.available()) Serial.read();
    for (int i = 0; i < mem.getTotalBlocks(); i++){
      if (i <= mem.getTotalEntryCount()){
        Serial.print("Entry #: ");
        Serial.print(mem.getTotalEntryCount() - i);
        mem.load(i * -1);
        unsigned int sig = data[0];
        sig <<= 8;
        sig |= data[1];
        Serial.print("   A0: ");
        Serial.print(sig);
        sig = data[2];
        sig <<= 8;
        sig |= data[3];
        Serial.print("   A1: ");
        Serial.println(sig);
      }
    }
  }
}
