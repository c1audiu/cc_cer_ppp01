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

void setup() {
  Serial.begin(9600);

  // display DFR0486
  // u8g.setRot180();  // flip screen

}

void loop() {
  // flow meter UF25B
  rawReading1 = analogRead(analogPin1);
  voltageReading1 = rawReading1 * (5.0 / 1023.0);
  if (voltageReading1 >= 0.2) {
    unitReading1 = (voltageReading1 - 0.2) / 0.2; // 0.2V offset; 0.2V = 1L/min
  }
  else {
    unitReading1 = 0;
  }

  //Serial.print(rawReading1); Serial.println(" ADC");
  //Serial.print(voltageReading1); Serial.println(" volts");
  //Serial.print(unitReading1); Serial.println(" L/min");

  // pressure transmitter XMEP250BT11F
  rawReading2 = analogRead(analogPin2);
  voltageReading2 = rawReading2 * (5.0 / 1023.0);
  if (voltageReading2 >= 0.5) {
    unitReading2 = (voltageReading2 - 0.5) / 0.016; // 0.5V offset; 0.016V = 1BAR
  }
  else {
    unitReading2 = 0;
  }

  //Serial.print(rawReading2); Serial.println(" ADC");
  //Serial.print(voltage1); Serial.println(" volts");
  //Serial.print(pressure1); Serial.println(" BAR");

  // print to display
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_fur11);
    u8g.drawStr( 0, 20, "L/min ");
    u8g.setPrintPos(64, 20);
    u8g.print(unitReading1);
    u8g.drawStr( 0, 60, "BAR ");
    u8g.setPrintPos(64, 60);
    u8g.print(unitReading2);
  } while ( u8g.nextPage() );
}
