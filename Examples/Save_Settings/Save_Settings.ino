/*
  Example firmware for EEPROMlevel library
  Written by Tyler Gerritsen
  vtgerritsen@gmail.com
  www.td0g.ca

DESCRIPTION:
  This firmware stores the settings state from previous sessions
  One byte is used to store eight settings
  To enter a setting, type lowercase a - h to clear and Uppercase A - H to set.

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
#define ARRAY_SIZE 1
uint8_t data[ARRAY_SIZE];
EEPROMlevel mem(data, ARRAY_SIZE, 0);

void setup() {
  Serial.begin(9600);
  mem.load();
  printSettings();
}

void loop() {
//If serial character is recieved, then set or clear the setting
  if (Serial.available()){
    char thisSetting = Serial.read();
    if (thisSetting >= 'a' and thisSetting <= 'h'){ //Lowercase - clear the setting
      thisSetting -= 'a';
      bitClear(data[0],(int)thisSetting);
      printSettings();
      mem.save(); //Don't need to check if the settings have changed, this will do it for us
    }
    else if (thisSetting >= 'A' and thisSetting <= 'H'){
      thisSetting -= 'A';
      bitSet(data[0],(int)thisSetting);
      printSettings();
      mem.save(); //Don't need to check if the settings have changed, this will do it for us
    }
  }
}

void printSettings(){
  for (byte i = 0; i < 7; i++){
    char c;
    if (data[0] & (1 << i))c = 'A' + i;
    else c = 'a' + i;
    Serial.print(c);
  }
  Serial.println();
}
