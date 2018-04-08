#include <SPI.h>
#include <NRF24.h>

NRF24 radio;
const int addr = 0xD2;

void setup() {
	Serial.begin(115200);
	radio.begin(9, 10);
  radio.setPowerAmplificationLevel(NRF24_PA_MAX);
  radio.setDataRate(NRF24_250KBPS);
  
  Serial.println(F("RX"));

	// set our address
	// note the address of the received, we'll need it later on!
	radio.setAddress(addr);
	radio.startListening();
}

void loop() {
  if (radio.available()) {
		char buf[32+1];
		uint8_t numBytes = radio.read(buf, sizeof(buf));
    buf[numBytes]='\0';
		Serial.print(F("Received: "));
    Serial.print(numBytes); Serial.print(F(", ")); Serial.println(buf);
	}
}
