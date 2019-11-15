// Gravity: Analog Voltage Divider V2 test

const int analogPin1 = A6;
int rawReading1 = 0;
float voltageReading1 = 0;
float unitReading1 = 0;

void setup()
{
  Serial.begin(9600);
}
void loop() {
  rawReading1 = analogRead(analogPin1);
  voltageReading1 = rawReading1 * (5000 / 1024.0);
  unitReading1 = rawReading1 / 40.4; // calibrated value; 1024/25V = 40.96
 
  //Serial.print(rawReading1); Serial.println(" ADC");
  //Serial.print(voltageReading1); Serial.println(" volts");
  Serial.print(unitReading1); Serial.println(" volts");
}
