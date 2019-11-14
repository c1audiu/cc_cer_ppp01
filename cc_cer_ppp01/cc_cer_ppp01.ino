// display DFR0486
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

// flow meter UF25B
int analogPin1 = A6;
int rawReading1 = 0;
float voltageReading1 = 0;
float unitReading1 = 0;

// pressure transmitter XMEP250BT11F
int analogPin2 = A7;
int rawReading2 = 0;
float voltageReading2 = 0;
float unitReading2 = 0;

void readFlow() {
  // read ADC 0-1023
  rawReading1 = analogRead(analogPin1);;

  // debug raw reading
  //Serial.print(rawReading1); Serial.println(" ADC");

  // convert analog input to voltage
  voltageReading1 = rawReading1 * (5.0 / 1023.0);

  // debug voltage reading
  Serial.print(voltageReading1); Serial.println(" volts");

  // convert volts to L/min;
  unitReading1 = (voltageReading1 - 0.2) / 0.2; // 0.2V offset; 0.2V = 1L/min

  // debug flow reading
  //Serial.print(unitReading1); Serial.println(" L/min");

  // print to display
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_fur11);
    u8g.drawStr( 0, 20, "Raw1 ");
    u8g.setPrintPos(64, 20);
    u8g.print(rawReading1);
  } while ( u8g.nextPage() );
}

void readPressure() {
  // read ADC 0-1023
  rawReading2 = analogRead(analogPin2);;

  // debug raw reading
  Serial.print(rawReading2); Serial.println(" ADC");

  // convert analog input to voltage
  voltageReading2 = rawReading2 * (5.0 / 1023.0);

  // debug voltage reading
  //Serial.print(voltage1); Serial.println(" volts");

  // convert volts to bars; 0.5V offset; 16mV / BAR
  unitReading2 = voltageReading2 * 0.016; // 0.016V = 1BAR

  // debug pressure reading
  //Serial.print(pressure1); Serial.println(" BAR");

  // print to display
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_fur11);
    u8g.drawStr( 0, 50, "Raw2 ");
    u8g.setPrintPos(64, 50);
    u8g.print(rawReading2);
  } while ( u8g.nextPage() );
}

void setup() {
  Serial.begin(9600);

  // display DFR0486
  // u8g.setRot180();  // flip screen

}

void loop() {
  readFlow();
  readPressure();
}
