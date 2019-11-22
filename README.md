# Hardware
#### Arduino Mega 2560 Rev3 [>info](https://store.arduino.cc/arduino-mega-2560-rev3)
#### Gravity: Digital 10A Relay Module [>info](https://www.dfrobot.com/product-1572.html)
#### Gravity: Digital 5A Relay Module [>info](https://www.dfrobot.com/product-64.html)
#### Gravity: I2C OLED-2864 Display [>info](https://www.dfrobot.com/product-1576.html)
#### Gravity: Terminal Sensor Adapter V2.0 [>info](https://www.dfrobot.com/product-203.html)
#### Gravity: Analog Voltage Divider V2 [>info](https://www.dfrobot.com/product-90.html) 
#### pressure transmitter - XMEP - 7500psi - 1/4"18NPT - 0,5..4,5V [>info](https://www.se.com/uk/en/product/XMEP7K5PT130/pressure-transmitter---xmep---7500psi---1-4%2218npt---0%2C5..4%2C5v---deutsch---set-1/)
#### pressure transmitter - XMEP - 250bar - G1/4A male - 0,5..4,5V [>info](https://www.se.com/uk/en/product/XMEP250BT11F/pressure-transmitter---xmep---250bar---g1-4a-male---0%2C5..4%2C5v---deutsch---set-1/)
 
# Software Calculations
#### Arduino Mega 
* 10bit analog resolution (1024 different values)
* analog input voltage 0 - 5 V ; 0 - 5000 mV
* 5/1024 = 0.0048828125 V / unit ; 5000/1024 = 4.8828125 mV / unit

#### Pressure transmitter XMEP 250 BAR
* range 0 - 250 BAR
* output voltage 0.5-4.5 V ; 500-4500 mV
* 4/250 = 0.016 V / BAR ; 4000/250 = 16 mV / BAR

#### Pressure transmitter XMEP 7500 PSI
* range 0 - 517 BAR
* output voltage 0.5 - 4.5 V ; 500-4500 mV 
* 4/517 = 0.0077 V / BAR ; 4000/517 = 7.7369 mV / BAR

#### Flow meter UF25B
* range 0.2 - 25 L/min
* output voltage ; 0 - 5000 mV 
* 5/25 = 0.2 V / L/min ; 5000/25 = 200 mV / L/min

# Connections
* rotation sensor for menu selection -> Arduino Analog Pin 6 
* rotation sensor for pressure limit selection -> Arduino Analog Pin 7
* push button for pressure limit set -> Arduino Digital Pin 41
* toggle switch for monitor on/off -> Arduino Digital Pin 42
* pressure sensor 1 -> Arduino Analog Pin 9
* flow sensor 1 -> Arduino Analog Pin 8
* voltage sensor 1 (trigger detection) -> Arduino Analog Pin 10
* relay 1 - > Arduino Digital Pin 43


