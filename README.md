# Hardware
#### Arduino Mega 2560 Rev3 [>info](https://store.arduino.cc/arduino-mega-2560-rev3)
#### Gravity: Digital 10A Relay Module [>info](https://www.dfrobot.com/product-1572.html)
#### Gravity: Digital 5A Relay Module [>info](https://www.dfrobot.com/product-64.html)
#### Gravity: I2C 16x2 Arduino LCD with RGB Backlight Display [>info](https://www.dfrobot.com/product-1609.html)
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

# Arduino Connections
* Analog Pin 6 -> rotation sensor (menu selection)   
* Analog Pin 7 -> rotation sensor (pressure limit selection)  
* Analog Pin 8 -> flow sensor 1
* Analog Pin 9 -> pressure sensor 1 
* Analog Pin 10 -> voltage sensor 1 (trigger detection)

* Digital Pin 41 -> push button (set pressure limit)  
* Digital Pin 42 -> toggle switch (monitor on/off) 
* Digital Pin 43 -> relay 1
