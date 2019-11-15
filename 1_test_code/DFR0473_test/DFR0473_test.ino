// Gravity: Digital 10A Relay Module test

const int relayPin1 = 9;

void setup() {
  pinMode(relayPin1, OUTPUT);
}

void loop() {
  digitalWrite(relayPin1, HIGH);   //Turn on relay
  //digitalWrite(relayPin1, LOW);   //Turn off relay
}
