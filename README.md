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

I went a bit further adding some things:

  * Battery monitor with a voltage divider that is switched off when it's not being used.
  
  * Turning the radio and the moisture sensor off when they are not being used.
  
  * Some sort of high level addressing to tell different sensors apart. Sensor address is a random 16 bit hex number included in the message. The address is random but persistent.
