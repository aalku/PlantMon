# PlantMon
Plant monitoring system for home.

I wanted a cheap wireless soil moisture sensor that run on batteries for weeks or months so I made one.

The data exploitation is a work in progress but I read the data and make it get to a computer so I don't expect any problems with it.

Main pieces:

  * I'm using this kind of sensor but I bought the cheapest I found:
    https://www.dfrobot.com/wiki/index.php/Capacitive_Soil_Moisture_Sensor_SKU:SEN0193

  * Mine might be a clone. I think dfrobot deseve at least the credit.

  * To transmit the data I'm using a NRF24l01 transreceiver.

  * To control it all I'm using an Arduino Pro Mini clone (based on ATmega328P) roghly modified to consume less power, including using a single 3.3v power regulator.

  * I'm using a 5V Arduino Pro Mini with 16 MHz clock and you shouldn't run ATmega328P at 16 MHz with 3.3v so please use a 3.3v Arduino Pro Mini at 8 Mhz to follow the specs. Do as I say, not as I do. But mine is working fine at 16MHz with 3.3v :-)

Brief explanation:

  * Just wire the radio (both transmitter and receiver) as told here:
    https://forum.arduino.cc/index.php?topic=421081.0
    
  * And try it with these library and examples:
    https://github.com/akupila/NRF24
    
  * Then (or first) connect the mousture sensor. Join both things and transmit the measured value and you got it.
  
  * Apply low-power modifications:
    http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/
    
  * Use code and libraries to monitor only every 5 minutes and sleep between cycles.
    I'm using this library: https://github.com/rocketscream/Low-Power
    You have to sleep in a loop (8s max each iteration). You have to be careful waiting some time (delay) before or after sleep. If you write to the Serial and then go to sleep immediately then you might be interrupting the Serial work as it is asynchronous. After sleep you might wait for VCC or references to stabilize before doing some things too.

I went a bit further adding some things:

  * Battery monitor with a voltage divider. The battery status is transmitted too.
  
  * Turning the radio and the moisture sensor off when they are not being used. I leave VCC connected to them and open the circuit to disconnect them on the GND side because I was working with a 5V Arduino for the prototype and radio VCC must not exceed 3.3v so I let the 3.3v fixed and used GPIO as on/off ground. A GPIO configured as input is high impedance so the circuit is open, to turn the radio or sensor on I switch the pin to LOW OUTPUT to act as ground. Of course you have to reset the radio after turning it on. I wait some time for the voltage to stabilize before actual use. I use two pins to get more mA in case it's needed. I turn off the voltage divider too the same way.
  
  * Some sort of high level addressing to tell different sensors apart. Sensor address is a random 16 bit hex number included in the message. The address is random but persistent. I made a circuit/program to select a new random address by keeping a button pressed some time while turning on. The address is function of the time the button is down in milliseconds.
