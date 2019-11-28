// lcd
#include <Wire.h>
#include <DFRobot_RGBLCD.h>
DFRobot_RGBLCD lcd(16, 2);

// switches
#include <Button.h>
const int pinSwitchSet = 41; // push switch for setting the pressure limits
Button buttonSet = (pinSwitchSet);

const int pinSwitchMonitor = 42; // push switch for changing between monitor on (read, compare with limits and to the logic) and monitor off (just read the values)
bool buttonMonitor = 0;

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
int limitHV = 0; // higher limit for pressure
int limitLV = 0; // lower limit for pressure
int limitVoltage = 3; // limit for voltage received from timed relay
float limitFlow = 0.0; // limit for water flow
int pressureFlag = 0;
int triggerFlag = 0;
int flowFlag = 0;
int monitFlag = 0;
int statusFlag = 0;
int errorFlag = 0;
unsigned long startMillis = millis(); // used for calculating the skip time for pressure monit
unsigned long currentMillis = 0; // used for calculating the skip time for pressure monit
unsigned long triggerMillis = 0; // used for calculating the skip time for pressure monit
const unsigned long skipMillis = 2000; // skip time for pressure monit

void flowRead() {
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
  flowRead();

  // flow meter UF25B 1
  if (unitFlowSensor1 <= limitFlow) {
    flowFlag = 1; // flow limit ok
  }
  else {
    flowFlag = 2; // flow limit nok
  }
}

void pressureRead() {
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
  pressureRead();
  if (limitLV <= round(unitPressureSensor1) && round(unitPressureSensor1) <= limitHV) {
    pressureFlag = 1; // pressure limits ok
  }
  else {
    pressureFlag = 2; // pressure limits nok
  }
}

void pressureMonit() {
  pressureLimit();
  triggerMonit();
  
  if (triggerFlag == 1 && pressureFlag == 1) {
    monitFlag = 1; // trigger on; the limits can be monitored; limits ok
  }
  else if (triggerFlag == 1 && pressureFlag == 2) {
    monitFlag = 2; // trigger on; the limits can be monitored; limits nok
  }
  else {
    monitFlag = 3; // monitor off or/and trigger off
  }
}

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
      triggerFlag = 1; // trigger on, the limits can be monitored
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

void statusMonit() {
  flowMonit();
  pressureMonit();

  if (!buttonMonitor) { // monitor ON
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
    errorFlag = 1; // flag to stop the monitor if error occurs
  }
  else if (errorFlag == 1 && buttonMonitor) { // reset error flag with monitor switch
    errorFlag = 0;
  }
}

void inputMenu() {
  // rotary switch 1
  rawSwitchMenu = analogRead(pinSwitchMenu);
  menuSelection = rawSwitchMenu / 204.8; // 1024/5 menu items

  // rotary switch 2
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
  flowRead();
  pressureRead();

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
      lcd.print(limitLV);
      if (buttonSet.pressed()) {
        limitLV = unitSwitchLimit;
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
      lcd.print(limitHV);
      if (buttonSet.pressed()) {
        limitHV = unitSwitchLimit;
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(unitSwitchLimit);
      break;

    default: // show monitor state and status code
      lcd.setCursor(0, 0);
      lcd.print("Monitor  ->");
      if (buttonMonitor) {
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

  // switches
  buttonSet.begin();

  // relays
  pinMode(pinRelayWater1, OUTPUT);
  pinMode(pinRelayPower1, OUTPUT);
}

void loop() {
  lcdMenu();
  errorMonit();

  if (errorFlag != 1) {
    digitalWrite(pinRelayWater1, HIGH);   // switch on relay
    digitalWrite(pinRelayPower1, HIGH);
  }
  else {
    digitalWrite(pinRelayWater1, LOW);   // switch off relay
    digitalWrite(pinRelayPower1, LOW);
  }

  // debug
  //Serial.print(unitFlowSensor1); Serial.print(">="); Serial.println(limitFlow);
  //Serial.print(limitLV); Serial.print("<="); Serial.print(round(unitPressureSensor1)); Serial.print("<="); Serial.println(limitHV);
  //Serial.println(pressureFlag);
  //Serial.println(flowFlag);
  //Serial.println(monitFlag);
  //Serial.println(errorFlag);
}
