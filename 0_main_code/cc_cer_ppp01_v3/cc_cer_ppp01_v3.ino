// i2c lcd
#include <Wire.h>
#include <DFRobot_RGBLCD.h>
DFRobot_RGBLCD lcd(16, 2);

// rotation sensor
const int analogPin1 = A6;
const int analogPin2 = A7;
int rawReading1 = 0;
int rawReading2 = 0;
int voltageReading2 = 0;
int unitReading2 = 0;

// push buttons
#include <Button.h>
const int digitalPin1 = 41;
const int digitalPin2 = 42;
bool digitalReading1 = 0;
bool digitalReading2 = 0;
//int currentButtonState = 0;
//int previousButtonState = 0;
Button button1 = (digitalPin1); // limit button
Button button2 = (digitalPin2); // monit button

// logic
int currentMenuSelection = 0;
int previousMenuSelection = 0;
int menuSelection = 0;
int currentReadingDisplay = 0;
int previousReadingDisplay = 0;
int limitHV = 0;
int limitLV = 0;
int limitVoltage = 3; // limit for voltage read from external relay
float limitFlow = 0.0;
int pressureFlag = 0;
int triggerFlag = 0;
int flowFlag = 0;
int monitFlag = 0;
int statusFlag = 0;
int errorFlag = 0;
unsigned long startMillis = millis();
unsigned long currentMillis = 0;
unsigned long triggerMillis = 0;
const unsigned long skipMillis = 2000; // skip time for pressure monit

// flow meter UF25B 1
const int analogPin3 = A8;
int rawReading3 = 0;
float voltageReading3 = 0;
float unitReading3 = 0; // water flow [l/min]

// pressure transmitter XMEP250BT11F 1
const int analogPin4 = A9;
int rawReading4 = 0;
float voltageReading4 = 0;
float unitReading4 = 0; // water high pressure [BAR]

// voltage divider 1
const int analogPin5 = A10;
int rawReading5 = 0;
//int voltageReading3 = 0;
int unitReading5 = 0; // max 25V

// relay 1
const int relayPin1 = 43;

void flowRead() {
  // flow meter UF25B 1
  rawReading3 = analogRead(analogPin3);
  voltageReading3 = rawReading3 * (5000 / 1024.0);
  if (voltageReading3 >= 200) {
    unitReading3 = (voltageReading3 - 200) / 200; // calculated value -> 200mV offset; 200mV = 1L/min
  }
  else {
    unitReading3 = 0;
  }
}

void flowMonit() {
  flowRead();

  if (unitReading3 <= limitFlow) {
    flowFlag = 1; // limit is OK
  }
  else {
    flowFlag = 2; // limit is NOK
  }
}

void pressureRead() {
  // pressure transmitter XMEP250BT11F 1
  rawReading4 = analogRead(analogPin4);
  voltageReading4 = rawReading4 * (5000 / 1024.0);
  if (voltageReading4 >= 500) {
    unitReading4 = (voltageReading4 - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitReading4 = 0;
  }
}

void pressureLimit() {
  lcdMenu();
  pressureRead();
  if (limitLV <= round(unitReading4) && round(unitReading4) <= limitHV) {
    pressureFlag = 1; // limits are OK
  }
  else {
    pressureFlag = 2; // limits are NOK
  }
}

void pressureMonit() {
  pressureLimit();
  triggerMonit();
  if (triggerFlag == 1 && pressureFlag == 1) {
    monitFlag = 1; // pressure is OK
  }
  else if (triggerFlag == 1 && pressureFlag == 2) {
    monitFlag = 2; // pressure is NOK
  }
  else {
    monitFlag = 3; // don't care
  }
}

void triggerRead() {
  // voltage divider 1
  rawReading5 = analogRead(analogPin5);
  //voltageReading5 = rawReading5 * (5000 / 1024.0);
  unitReading5 = rawReading5 / 40.4; // <- calibrated value; calculated value -> 1024/25V = 40.96
}

void triggerMonit() {
  triggerRead();
  if (limitVoltage < unitReading5) {
    currentMillis = millis();
    triggerMillis = currentMillis - startMillis;
    if (triggerMillis >= skipMillis) {
      triggerFlag = 1; // the pressure can be monitored
    }
    else {
      triggerFlag = 2; // to early to start the pressure monitor
    }
  }
  else {
    startMillis = millis();
    triggerFlag = 0;
  }
}

void statusMonit() {
  flowMonit();
  pressureMonit();

  if (!digitalReading2) { // monitor ON
    if (monitFlag == 1 && flowFlag == 1) {
      statusFlag = 1;   // Turn on relay
    }
    else if (monitFlag == 3 && flowFlag == 1) {
      statusFlag = 2;    // Turn on relay
    }
    else {
      statusFlag = 3;   // Turn off relay
    }
  }
  else { // monitor OFF
    statusFlag = 4;    // Turn on relay
  }
}

void errorMonit() {
  statusMonit();
  if (statusFlag == 3) {
    errorFlag = 1;
  }
  else if (errorFlag == 1 && digitalReading2 == 1){
    errorFlag = 0;
  }
}

void inputMenu() {
  // rotation sensor 1
  rawReading1 = analogRead(analogPin1);
  menuSelection = rawReading1 / 204.8; // 1024 / menu items

  // rotation sensor 2
  rawReading2 = analogRead(analogPin2);
  voltageReading2 = rawReading2 * (5000 / 1024);
  if (voltageReading2 >= 500) {
    unitReading2 = (voltageReading2 - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitReading2 = 0;
  }

  // push button 2
  digitalReading2 = digitalRead(digitalPin2);
}

void lcdMenu() {
  inputMenu();
  flowRead();
  pressureRead();

  // i2c lcd
  lcd.setRGB(255, 255, 255);

  // lcd display menu
  previousReadingDisplay = currentReadingDisplay;
  previousMenuSelection = currentMenuSelection;
  currentMenuSelection = menuSelection;

  if (currentMenuSelection != previousMenuSelection) {
    lcd.clear();
  }

  switch (currentMenuSelection) {
    case 1:
      // refresh display
      currentReadingDisplay = rawReading3;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Flow");
      lcd.setCursor(0, 1);
      lcd.print(unitReading3);
      lcd.setCursor(13, 1);
      lcd.print("l/m");
      break;

    case 2:
      // refresh display
      currentReadingDisplay = rawReading4;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Pressure");
      lcd.setCursor(0, 1);
      lcd.print(unitReading4);
      lcd.setCursor(13, 1);
      lcd.print("BAR");
      break;

    case 3:
      // refresh display
      currentReadingDisplay = rawReading2;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Limit LV");
      lcd.setCursor(10, 0);
      lcd.print(limitLV);
      if (button1.pressed()) {
        limitLV = unitReading2;
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(unitReading2);
      break;

    case 4:
      // refresh display
      currentReadingDisplay = rawReading2;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Limit HV");
      lcd.setCursor(10, 0);
      lcd.print(limitHV);
      if (button1.pressed()) {
        limitHV = unitReading2;
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(unitReading2);
      break;

    default:
      lcd.setCursor(0, 0);
      lcd.print("Monitor  ->");
      if (digitalReading2) {
        lcd.setCursor(13, 0);
        lcd.print("Off");
      }
      else {
        lcd.setCursor(13, 0);
        lcd.print("On ");
      }
      lcd.setCursor(0, 1);
      lcd.print("Status   ->");
      lcd.setCursor(13, 1);
      lcd.print(monitFlag);
      break;
  }
}

void setup() {
  // debug
  Serial.begin(9600);

  // i2c lcd
  lcd.init();

  // push buttons
  button1.begin();
  button2.begin();

  // relay 1
  pinMode(relayPin1, OUTPUT);
}

void loop() {
  lcdMenu();
  //flowMonit();
  //pressureMonit();
  //statusMonit();
  errorMonit();

  if (errorFlag != 1) {
    digitalWrite(relayPin1, HIGH);   // Turn on relay
  }
  else {
    digitalWrite(relayPin1, LOW);   // Turn off relay
  }

  // debug
  //Serial.print(unitReading3); Serial.print(">="); Serial.println(limitFlow);
  //Serial.print(limitLV); Serial.print("<="); Serial.print(round(unitReading4)); Serial.print("<="); Serial.println(limitHV);
  //Serial.println(pressureFlag);
  //Serial.println(flowFlag);
  //Serial.println(monitFlag);
  Serial.println(errorFlag);

}
