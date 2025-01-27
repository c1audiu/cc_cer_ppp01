// sd
#include <SPI.h>
#include <SD.h>

File myFile;

// switches
#include <Button.h>
const int pinSwitchSet = 41; // push switch for setting the pressure limits
Button buttonSet = (pinSwitchSet);

const int pinSwitchLimit = A7; // rotary switch for selecting the pressure limits
int rawSwitchLimit = 0; // raw reading
int voltageSwitchLimit = 0; // voltage conversion
int unitSwitchLimit = 0;  // measurement unit conversion [BAR]

void setRead() {
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
  Serial.begin(9600);

  // sd
  SD.begin(53);
}

void loop() {
  setRead();

  if (buttonSet.pressed()) {
    SD.remove("high1.txt");
    myFile = SD.open("high1.txt", FILE_WRITE);
    if (myFile) {
      Serial.print("Writing to high1.txt...");
      myFile.println(unitSwitchLimit);
      // close the file:
      myFile.close();
      Serial.println("done.");
    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening high1.txt");
    }
  }
}
