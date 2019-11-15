// display DFR0486
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

// flow meter UF25B 1
const int analogPin1 = A6;
int rawReading1 = 0;
float voltageReading1 = 0;
float unitReading1 = 0;

// pressure transmitter XMEP250BT11F 1
const int analogPin2 = A7;
int rawReading2 = 0;
float voltageReading2 = 0;
float unitReading2 = 0;

// voltage divider 1
const int analogPin3 = A8;
int rawReading3 = 0;
//int voltageReading3 = 0;
int unitReading3 = 0;

// relay 1
const int relayPin1 = 9;
String stateRelay = "On";

// monit
const int barHigh1 = 100;
const int barLow1 = 0;
const int flowHigh1 = 10;

void setup() {
  // serial debug
  Serial.begin(9600);

  // display DFR0486
  //u8g.setRot180();  // flip screen

  // relay 1
  pinMode(relayPin1, OUTPUT);
}

void loop() {

  //Serial.println(stateRelay);

  // flow meter UF25B 1
  rawReading1 = analogRead(analogPin1);
  voltageReading1 = rawReading1 * (5000 / 1024.0);
  if (voltageReading1 >= 200) {
    unitReading1 = (voltageReading1 - 200) / 200; // calculated value -> 200mV offset; 200mV = 1L/min
  }
  else {
    unitReading1 = 0;
  }

  // pressure transmitter XMEP250BT11F 1
  rawReading2 = analogRead(analogPin2);
  voltageReading2 = rawReading2 * (5000 / 1024.0);
  if (voltageReading2 >= 500) {
    unitReading2 = (voltageReading2 - 500) / 16; // calculated value -> 500mV offset; 16mV = 1BAR
  }
  else {
    unitReading2 = 0;
  }

  // voltage divider 1
  rawReading3 = analogRead(analogPin3);
  //voltageReading3 = rawReading3 * (5000 / 1024.0);
  unitReading3 = rawReading3 / 40.4; // <- calibrated value; calculated value -> 1024/25V = 40.96

  // monitor
  if (unitReading3 >= 10) {
    delay(3000);
    if (barLow1 < unitReading2 < barHigh1 || unitReading1 < flowHigh1) { // pressure on, monitor flow & pressure
      digitalWrite(relayPin1, HIGH); // turn on relay
    }
    else {
      digitalWrite(relayPin1, LOW);   // turn off relay
      stateRelay = "Off";
    }
  }
  else {
    if (unitReading1 < flowHigh1); // pressure off, monitor flow
    digitalWrite(relayPin1, LOW);   // turn off relay
    stateRelay = "Off";
  }


  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr( 0, 12, "L/min");
    u8g.setPrintPos(75, 12);
    u8g.print(unitReading1);
    u8g.drawStr( 0, 26, "BAR");
    u8g.setPrintPos(75, 26);
    u8g.print(unitReading2);
    u8g.drawStr( 0, 40, "Voltage");
    u8g.setPrintPos(75, 40);
    u8g.print(unitReading3);
    u8g.drawStr( 0, 54, "Relay");
    u8g.setPrintPos(75, 54);
    u8g.print(stateRelay);
  } while ( u8g.nextPage() );
}
