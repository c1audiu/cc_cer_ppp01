#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

void draw(void) {
  u8g.setFont(u8g_font_fur11);
  u8g.drawStr( 0, 36, "Hello World!");
}

void setup(void) {
  // flip screen
  // u8g.setRot180();
}

void loop(void) {

  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(50);
}
