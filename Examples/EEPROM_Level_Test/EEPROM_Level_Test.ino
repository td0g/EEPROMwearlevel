#include <EEPROMlevel.h>

#define TEST_NUMBER 63 //Perform all from 0 to 63
/*
          AverageMillis per save (DO NOT ENTER if errors occurred)
Test #    v0.3  v0.5/1.0
0         21    25
1         24    28
2         31    35
3         50    55

4         40    25
5         47    28
6         56    35
7         78    55

8         40    25
9         48    28
10        61    35
11        86    55

12        40    25
13        51    28
14        68    35
15        99    55

16        18    20
17        21    24
18        28    31
19        47    51

20        33    21
21        38    24
22        48    31
23        69    51

24        33    21
25        40    24
26        51    31
27        75    51

28        33    21
29        42    24
30        56    31
31        84    51

32        21    25
33        24    28
34        31    35
35        50    55

36        40    25
37        47    28
38        56    35
39        78    55

40        40    25
41        48    28
42        61    35
43        86    55

44        40    25
45        51    28
46        67    35
47        99    55

48        18    20
49        21    24
50        28    30
51        47    51

52        38    21
53        38    24
54        48    31
55        69    51

56        33    21
57        40    24
58        52    31
59        75    51

60        33    21
61        42    24
62        56    31
63        85    51
 */


#if (TEST_NUMBER & 0b00000011) == 0
  #define ARRAY_SIZE 2
#elif (TEST_NUMBER & 0b00000011) == 1
  #define ARRAY_SIZE 3
#elif (TEST_NUMBER & 0b00000011) == 2
  #define ARRAY_SIZE 5
#else
  #define ARRAY_SIZE 10
#endif

#if (TEST_NUMBER & 0b00001100) == 0
  #define CHECK_SIZE 0
#elif (TEST_NUMBER & 0b00001100) == 4
  #define CHECK_SIZE 1
#elif (TEST_NUMBER & 0b00001100) == 8
  #define CHECK_SIZE 2
#else
  #define CHECK_SIZE 4
#endif

#if (TEST_NUMBER & 0b00010000) == 0
  #define START_POS 0
#else
  #define START_POS 261
#endif

#if TEST_NUMBER & 0b00100000 == 0
  #define END_POS 800
#else
  #define END_POS 1100
#endif

uint8_t val[ARRAY_SIZE];
EEPROMlevel mem(val, ARRAY_SIZE, CHECK_SIZE, START_POS, END_POS);

uint8_t hist[ARRAY_SIZE][10];
unsigned long count = 0;
unsigned int x;

void setup() {
  Serial.begin(9600);
  Serial.print("TEST_NUMBER = ");
  Serial.println(TEST_NUMBER);
  Serial.print("ARRAY_SIZE = ");
  Serial.println(ARRAY_SIZE);
  Serial.print("CHECK_SIZE = ");
  Serial.println(CHECK_SIZE);
  Serial.print("START_POS = ");
  Serial.println(START_POS);
  Serial.print("END_POS = ");
  Serial.println(END_POS);
  Serial.println();
  for (int i = 0; i < EEPROM.length(); i++){
    EEPROM.write(i, 0);
  }
  delay(200);
}

void loop() {
  unsigned long errorCount = 0;
  unsigned long startTime = millis();
  while (count < 2000){
  for (byte i = 0; i < ARRAY_SIZE; i++){
    for (byte j = 9; j > 0; j--){
      byte t = hist[i][j-1];
      hist[i][j] = t;
    }
    byte t = random(256);
      if (!i) t &= 0b01111111;
    val[i] = t;
    hist[i][0] = t;
  }
  mem.save();
  mem.load();

  for (byte i = 0; i < ARRAY_SIZE; i++){
    if (val[i] != hist[i][0]) {
      errorCount ++;
      Serial.print("ERROR - val[");
      Serial.print(i);
      Serial.print("]=");
      Serial.print(val[i]);
      Serial.print("    hist[");
      Serial.print(i);
      Serial.print("][0]=");
      Serial.println(hist[i][0]);
      delay(1000);
    }
  }
  #if CHECK_SIZE == 0
    if (mem.getTotalEntryCount() != count % mem.getTotalBlocks()){
  #else
    if (mem.getTotalEntryCount() != count){
  #endif
      errorCount++;
      Serial.print("ERROR - gtec=");
      Serial.print(mem.getTotalEntryCount());
      Serial.print("  count=");
      Serial.println(count);
      delay(1000);
    }
  for (byte i = 0; i < ARRAY_SIZE; i++){
    for (int j = 0; j < 10; j++){
      if (j < count){
      mem.load(j * -1);
        if (val[i] != hist[i][j]){
          errorCount++;
          Serial.print("ERROR - rod(");
          Serial.print(j*-1);
          Serial.print(",");
          Serial.print(i);
          Serial.print(")=");
          Serial.print(val[i]);
          Serial.print("   his[");
          Serial.print(i);
          Serial.print("][");
          Serial.print(j);
          Serial.print("]=");
          Serial.println(hist[i][j]);
          delay(2000);
        }
      }
    }
  }
  Serial.print(count);
  Serial.println(" DONE");
  count++;
  }
  Serial.print("Total Errors: ");
  Serial.println(errorCount);
  Serial.print("Total Seconds: ");
  Serial.println((millis() - startTime)/1000);
  Serial.print("Average millis Per Entry: ");
  Serial.println((millis() - startTime) / count);
  while(1){};
}
