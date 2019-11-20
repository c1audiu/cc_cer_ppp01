// i2c lcd
#include <Wire.h>
#include <DFRobot_RGBLCD.h>
DFRobot_RGBLCD lcd(16, 2);

// rotation sensor
const int analogPin1 = A6;
const int analogPin2 = A7;
int rawReading1 = 0;
int rawReading2 = 0;

// push buttons
#include <Button.h>
const int digitalPin1 = 41;
const int digitalPin2 = 42;
bool digitalReading1 = 0;
bool digitalReading2 = 0;
int currentButtonState = 0;
int previousButtonState = 0;
Button button1 = (digitalPin1);
Button button2 = (digitalPin2);

// logic
int currentMenuSelection = 0;
int previousMenuSelection = 0;
int limitHV = 0;
int limitLV = 0;
int limitVoltage = 10; // limit for voltage read from external relay
float limitFlow = 0;
int limitMonit = 0;
int menuSelection = 0;
int currentReadingDisplay = 0;
int previousReadingDisplay = 0;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 5000;

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
const int relayPin1 = 9;

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

void pressureMonit() {
  triggerRead();
  if (limitVoltage < unitReading5) {
    currentMillis = millis();
    if (currentMillis - startMillis >= period) {
      if (limitLV <= unitReading4 <= limitHV){
        limitMonit = 1;       
      }
      startMillis = currentMillis;
    }
  }
}

void triggerRead() {
  // voltage divider 1
  rawReading5 = analogRead(analogPin5);
  //voltageReading5 = rawReading5 * (5000 / 1024.0);
  unitReading5 = rawReading5 / 40.4; // <- calibrated value; calculated value -> 1024/25V = 40.96
}

void lcdMenu() {
  flowRead();
  pressureRead();

  // rotation sensor
  rawReading1 = analogRead(analogPin1);
  rawReading2 = analogRead(analogPin2);

  // push button
  digitalReading2 = digitalRead(digitalPin2);

  // i2c lcd
  lcd.setRGB(255, 255, 255);

  // lcd display menu
  previousReadingDisplay = currentReadingDisplay;
  //currentReadingDisplay = rawReading2;
  previousMenuSelection = currentMenuSelection;
  currentMenuSelection = menuSelection;

  if (currentMenuSelection != previousMenuSelection) {
    lcd.clear();
  }

  menuSelection = rawReading1 / 204.8; // 1024 / menu items

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
      lcd.print("Limit HV");
      lcd.setCursor(10, 0);
      lcd.print(limitHV);
      if (button1.pressed()) {
        limitHV = rawReading2;
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(rawReading2);
      break;

    case 4:
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
        limitLV = rawReading2;
      }
      lcd.setCursor(0, 1);
      lcd.print("New Limit");
      lcd.setCursor(10, 1);
      lcd.print(rawReading2);
      break;

    default:
      lcd.setCursor(0, 0);
      lcd.print("Monitor Status");
      if (digitalReading2) {
        lcd.setCursor(0, 1);
        lcd.print("Off");
      }
      else {
        lcd.setCursor(0, 1);
        lcd.print("On ");
      }
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

  // logic
  startMillis = millis();
}

void loop() {
  triggerRead();
  lcdMenu();

  if (!digitalReading2) {
    if ( limitMonit = 0 ) { // monitor limits
      digitalWrite(relayPin1, HIGH);   //Turn on relay
      Serial.println("Relay ON, Pressure Ok");
    }
    else if (limitFlow == unitReading3) {
      digitalWrite(relayPin1, HIGH);   //Turn on relay
      Serial.println("Relay ON, Flow Ok");
    }
    else {
      digitalWrite(relayPin1, LOW);   //Turn off relay
      Serial.println("Relay Off, Monit On");
    }
  }
  else {
    digitalWrite(relayPin1, HIGH);   //Turn on relay
    Serial.println("Relay ON, Monit Off");
  }
}
