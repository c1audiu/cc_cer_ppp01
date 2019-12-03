/*
  Flag explaination
  
  triggerFlag = 1; // trigger on, settling time passed, the limits can be monitored
  triggerFlag = 2; // trigger on, to early to start the limits monitor

  flowFlag = 1; // flow limit ok
  flowFlag = 2; // flow limit nok

  pressureFlag = 1; // pressure limits ok
  pressureFlag = 2; // pressure limits nok

  monitFlag = 1; // trigger on, flow limit ok, pressure ok
  monitFlag = 2; // trigger on, flow limit ok, pressure nok
  monitFlag = 3; // trigger on, flow nok
  monitFlag = 4; // trigger off

  relayFlag = 1; // relay on
  relayFlag = 2; // relay off

  errorFlag = 1; // flag to stop and wait if error occurs
*/

// lcd
#include <Wire.h>
#include <DFRobot_RGBLCD.h>
DFRobot_RGBLCD lcd(16, 2);

// sd
#include <SPI.h>
#include <SD.h>
File sdLimitLow1;
File sdLimitHigh1;

// switches
#include <Button.h>
const int pinSwitchSet = 41; // push switch for setting the pressure limits
Button buttonSet = (pinSwitchSet);

const int pinSwitchMonitor = 42; // push switch for changing between monitor on (read, compare with limits and to the logic) and monitor off (just read the values)
int buttonMonitor = 0;

const int pinSwitchMenu = A6; // rotary switch for menu navigation
int rawSwitchMenu = 0; // raw reading

const int pinSwitchLimit = A7; // rotary switch for selecting the pressure limits
int rawSwitchLimit = 0; // raw reading
int voltageSwitchLimit = 0; // voltage conversion
int unitSwitchLimit = 0;  // measurement unit conversion [BAR]

// sensors
const int pinFlowSensor1 = A8; // analog flow meter 1
int rawFlowSensor1 = 0; // raw reading
float voltageFlowSensor1 = 0; // voltage conversion
float unitFlowSensor1 = 0; // measurement unit conversion [l/min]

const int pinPressureSensor1 = A9; // analog pressure transmitter 1
int rawPressureSensor1 = 0; // raw reading
float voltagePressureSensor1 = 0; // voltage conversion
float unitPressureSensor1 = 0; // measurement unit conversion [BAR]

const int pinTrigger1 = A10; // voltage divider 1; used to read the timed relay voltage that trigger the high-pressure gun
int rawTrigger1 = 0; // raw reading
int unitTrigger1 = 0; // measurement unit conversion [V];

// relays
const int pinRelayWater1 = 43; // relay 1 used to turn off the water valve for post 1
const int pinRelayPower1 = 28; // relay 2 used to turn off the power for  post 1

// logic
int menuSelection = 0; // used to calculate the menu selection from the rotary switch position
int currentMenuSelection = 0; // used for lcd refresh
int previousMenuSelection = 0; // used for lcd refresh
int currentReadingDisplay = 0; // used for lcd refresh
int previousReadingDisplay = 0; // used for lcd refresh
int limitHigh = 0; // higher limit for pressure
int limitLow = 0; // lower limit for pressure
int limitVoltage = 3; // limit for voltage received from timed relay
float limitFlow = 0.0; // limit for water flow
int pressureFlag = 0;
int triggerFlag = 0;
int flowFlag = 0;
int monitFlag = 0;
int relayFlag = 0;
int errorFlag = 0;
unsigned long startMillis = millis(); // used for calculating the skip time for pressure monit
unsigned long currentMillis = 0; // used for calculating the skip time for pressure monit
unsigned long triggerMillis = 0; // used for calculating the skip time for pressure monit
const unsigned long skipMillis = 2000; // skip time for pressure monit

void triggerRead() {
  // voltage divider 1
  rawTrigger1 = analogRead(pinTrigger1);
  //voltageReading5 = rawTrigger1 * (5000 / 1024.0);
  unitTrigger1 = rawTrigger1 / 40.4; // <- calibrated value; calculated value -> 1024/25V = 40.96
}

void triggerMonit() {
  triggerRead();
  if (limitVoltage < unitTrigger1) {
    currentMillis = millis();
    triggerMillis = currentMillis - startMillis;
    if (triggerMillis >= skipMillis) {
      triggerFlag = 1; // trigger on, settling time passed, the limits can be monitored
    }
    else {
      triggerFlag = 2; // trigger on, to early to start the limits monitor
    }
  }
  else {
    startMillis = millis();
    triggerFlag = 0;
  }
}

void flowRead1() {
  // flow meter UF25B 1
  rawFlowSensor1 = analogRead(pinFlowSensor1);
  voltageFlowSensor1 = rawFlowSensor1 * (5000 / 1024.0);
  if (voltageFlowSensor1 >= 200) {
    unitFlowSensor1 = (voltageFlowSensor1 - 200) / 200; // calculated value -> 200mV offset; 200mV = 1L/min
  }
  else {
    unitFlowSensor1 = 0;
  }
}

void flowMonit() {
  flowRead1();

  // flow meter UF25B 1
  if (unitFlowSensor1 <= limitFlow) {
    flowFlag = 1; // flow limit ok
  }
  else {
    flowFlag = 2; // flow limit nok
  }
}

void pressureRead1() {
  // pressure transmitter XMEP250BT11F 1
  rawPressureSensor1 = analogRead(pinPressureSensor1);
  voltagePressureSensor1 = rawPressureSensor1 * (5000 / 1024.0);
  if (voltagePressureSensor1 >= 500) {
    unitPressureSensor1 = (voltagePressureSensor1 - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitPressureSensor1 = 0;
  }
}

void pressureLimit() {
  lcdMenu();
  pressureRead1();
  if (limitLow <= round(unitPressureSensor1) && round(unitPressureSensor1) <= limitHigh) {
    pressureFlag = 1; // pressure limits ok
  }
  else {
    pressureFlag = 2; // pressure limits nok
  }
}

void pressureMonit() {
  triggerMonit();
  flowMonit();
  pressureLimit();

  if (triggerFlag == 1) { // trigger on, settling time passed, the limits can be monitored
    if (flowFlag == 1) {
      if  (pressureFlag == 1) {
        monitFlag = 1; // trigger on, flow limit ok, pressure ok
      }
      else {
        monitFlag = 2; // trigger on, flow limit ok, pressure nok
      }
    }
    else {
      monitFlag = 3; // trigger on, flow nok
    }
  }
  else {
    monitFlag = 4; // trigger off
  }
}

void relayMonit() {
  inputMenu();
  flowMonit();
  pressureMonit();

  if (buttonMonitor == 0) { // monitor button is on
    if (monitFlag == 1 || monitFlag == 4) {
      relayFlag = 1 ; // relay on
    }
    else { // limits nok
      relayFlag = 2; // relay off
    }
  }
  else { // monitor button is off
    relayFlag = 1; // relay on
  }
}

void errorMonit() { // function to block the relay if flow or pressure is nok
  relayMonit();

  if (relayFlag == 2) {
    errorFlag = 1; // flag to stop the monitor if error occurs
  }
  else if (errorFlag == 1 && buttonMonitor == 1) { // reset error flag with monitor switch off
    errorFlag = 0;
  }
}

void inputMenu() {
  // rotary switch for Menu selection
  rawSwitchMenu = analogRead(pinSwitchMenu);
  menuSelection = rawSwitchMenu / 204.8; // 1024/5 menu items

  // rotary switch for setting the pressure limits
  rawSwitchLimit = analogRead(pinSwitchLimit);
  voltageSwitchLimit = rawSwitchLimit * (5000 / 1024);
  if (voltageSwitchLimit >= 500) {
    unitSwitchLimit = (voltageSwitchLimit - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitSwitchLimit = 0;
  }

  // push button 2
  buttonMonitor = digitalRead(pinSwitchMonitor);
}

void lcdMenu() {
  inputMenu();
  flowRead1();
  pressureRead1();

  // i2c lcd
  lcd.setRGB(255, 255, 255);

  // refresh when the values change
  previousReadingDisplay = currentReadingDisplay;
  previousMenuSelection = currentMenuSelection;
  currentMenuSelection = menuSelection;

  if (currentMenuSelection != previousMenuSelection) {
    lcd.clear();
  }

  // menu options
  switch (currentMenuSelection) {

    case 1: // show flow from sensor 1
      // refresh display
      currentReadingDisplay = rawFlowSensor1;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Flow");
      lcd.setCursor(0, 1);
      lcd.print(unitFlowSensor1);
      lcd.setCursor(13, 1);
      lcd.print("l/m");
      break;

    case 2: // show pressure from sensor 1
      // refresh display
      currentReadingDisplay = rawPressureSensor1;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Pressure");
      lcd.setCursor(0, 1);
      lcd.print(unitPressureSensor1);
      lcd.setCursor(13, 1);
      lcd.print("BAR");
      break;

    case 3: // show and set low limit for pressure 1
      // refresh display
      currentReadingDisplay = rawSwitchLimit;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Limit LV");
      lcd.setCursor(10, 0);
      lcd.print(limitLow);
      if (buttonSet.pressed()) {
        if (SD.exists("limitLow1.txt")) {
          SD.remove("limitLow1.txt");
        }
        else {
          sdLimitLow1 = SD.open("limitLow1.txt", FILE_WRITE);
          if (sdLimitLow1) {
            sdLimitLow1.println(unitSwitchLimit);
            sdLimitLow1.close();
          }
        }
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(unitSwitchLimit);
      break;

    case 4: // show and set high limit for pressure 1
      // refresh display
      currentReadingDisplay = rawSwitchLimit;
      if (previousReadingDisplay - currentReadingDisplay >= 2) {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      lcd.print("Limit HV");
      lcd.setCursor(10, 0);
      lcd.print(limitHigh);
      if (buttonSet.pressed()) {
        if (SD.exists("limitHigh1.txt")) {
          SD.remove("limitHigh1.txt");
        }
        else {
          sdLimitHigh1 = SD.open("limitHigh1.txt", FILE_WRITE);
          if (sdLimitHigh1) {
            sdLimitHigh1.println(unitSwitchLimit);
            sdLimitHigh1.close();
          }
        }
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(unitSwitchLimit);
      break;

    default: // show monitor state and status code
      lcd.setCursor(0, 0);
      lcd.print("Monitor  ->");
      if (buttonMonitor == 1) {
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

  // lcd
  lcd.init();

  // sd
  SD.begin(53);

  // switches
  buttonSet.begin();
  pinMode(pinSwitchMonitor, INPUT);

  // relays
  pinMode(pinRelayWater1, OUTPUT);
  pinMode(pinRelayPower1, OUTPUT);
}

void loop() {
  lcdMenu();
  errorMonit();

  if (errorFlag == 0) { // switch on relays
    digitalWrite(pinRelayWater1, HIGH);
    digitalWrite(pinRelayPower1, HIGH);
  }
  else { // switch off relays
    digitalWrite(pinRelayWater1, LOW);
    digitalWrite(pinRelayPower1, LOW);
  }

  // debug
  //Serial.print(unitFlowSensor1); Serial.print(">="); Serial.println(limitFlow);
  //Serial.print(limitLow); Serial.print("<="); Serial.print(round(unitPressureSensor1)); Serial.print("<="); Serial.println(limitHigh);
  //Serial.print("triggerFlag == "); Serial.println(triggerFlag);
  //Serial.print("buttonMonitor == "); Serial.println(buttonMonitor);
  //Serial.print("pressureFlag == "); Serial.println(pressureFlag);
  //Serial.print("flowFlag == "); Serial.println(flowFlag);
  //Serial.print("monitFlag == "); Serial.println(monitFlag);
  //Serial.print("errorFlag == "); Serial.println(errorFlag);
  //Serial.print("relayFlag == "); Serial.println(triggerFlag);
}
