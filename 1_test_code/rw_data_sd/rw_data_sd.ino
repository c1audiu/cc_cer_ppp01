// read data from rotary switch and when the button switch is pressed save it to SD card.
// read the data from the SD card in to a variable.

// sd
#include <SPI.h>
#include <SD.h>
File sdLowIn1;
File sdLowOut1;
int limitLow1 = 0;

// switches
#include <Button.h>
const int pinSwitchSet = 41; // push switch for setting the pressure limits
Button buttonSet = (pinSwitchSet);

const int pinSwitchLimit = A7; // rotary switch for selecting the pressure limits
int rawSwitchLimit = 0; // raw reading
int voltageSwitchLimit = 0; // voltage conversion
int unitSwitchLimit = 0;  // measurement unit conversion [BAR]

void inputMenu() {
  // rotary switch for setting the pressure limits
  rawSwitchLimit = analogRead(pinSwitchLimit);
  voltageSwitchLimit = rawSwitchLimit * (5000 / 1024);
  if (voltageSwitchLimit >= 500) {
    unitSwitchLimit = (voltageSwitchLimit - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitSwitchLimit = 0;
  }
}

void setup() {
  // debug
  Serial.begin(9600);

  // sd
  SD.begin(53);

  // switches
  buttonSet.begin();
}

void loop() {
  inputMenu();
  if (buttonSet.pressed()) { // write the lower limit on the sd card
    SD.remove("low1.txt");
    File sdLowIn1 = SD.open("low1.txt", FILE_WRITE);
    if (sdLowIn1) {
      sdLowIn1.println(unitSwitchLimit);
      sdLowIn1.close();
    }
  }

  else {
    File sdLowOut1 = SD.open("low1.txt");
    if (sdLowOut1) {
      limitLow1 = sdLowOut1.parseInt();
      sdLowOut1.close();
    }
  }
  Serial.println(limitLow1);
}
