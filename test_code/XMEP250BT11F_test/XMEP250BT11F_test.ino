// ultrasonic flow meter UF25B read

int sensor1Pin = A6;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read ADC 0-1023
  int reading1 = analogRead(sensor1Pin);;

  // print the analog input reading
  Serial.print(reading1); Serial.println(" ADC units");

  // convert analog input to voltage
  float voltage1 = reading1 * 5.0;
  voltage1 /= 1024.0;

  // print the voltage reading
  Serial.print(voltage1); Serial.println(" volts");

  // convert volts to bars; 0.5V offset; 16mV / BAR
  float pressure1 = (voltage1 - 0.5) * (0.016 * 1000);

  // print the pressure reading
  Serial.print(pressure1); Serial.println(" BAR");
}
