#include <LowPower.h>

#include <SPI.h>
#include <NRF24.h>

#include <Vcc.h>

#include <EEPROM.h>

const float minVIN = 4.05; // Make it sleep forever if VIN < this to try not to damage batteries. Use 0 if batteries are not rechargable.
// TODO Allow an input pin to select if battery is rechargable.

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);

const int radioAddr = 0xD2;
char msg[32+1] = "";

const int PIN_RADIO_GND_A = 8;
const int PIN_RADIO_GND_B = 7;
const int PIN_WATER_GND_A = 6;
const int PIN_WATER_GND_B = 5;
const int PIN_VIN_SENSE_GND = 4;
const int PIN_ADDR = 2;
const int PIN_ANALOG_A = A0;
const int PIN_ANALOG_VIN = A1;

unsigned char seq = 0;

// unsigned long interval = 15*60 // 15m
unsigned long interval = 1;

unsigned long addr;

long calMax = 950;
long calMin = 300;

#define DEBUG

void setup() {
  #ifdef DEBUG
	Serial.begin(115200);
  Serial.println("HI! TX here.");
  delay(100);
  #endif

  // Get addr and update it when PIN_ADDR is held low.
  EEPROM.get(0, addr);  
  pinMode(PIN_ADDR, INPUT_PULLUP);
  delay(100);
  if (digitalRead(PIN_ADDR)==LOW) {
    while (digitalRead(PIN_ADDR)==LOW) {
      // Active wait
    }
    addr = addr * 983 + millis();
    EEPROM.put(0, addr);
  }
}

void loop() {
  
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  delay(200);
  float v = vcc.Read_Volts();
  v = v / (5.14/5.06); // Calibration = read/actual

  delay(100);
  pinMode(PIN_RADIO_GND_A, OUTPUT);
  pinMode(PIN_RADIO_GND_B, OUTPUT);
  digitalWrite(PIN_RADIO_GND_A, LOW);
  digitalWrite(PIN_RADIO_GND_B, LOW);
  delay(500);
  NRF24 radio;
  radio.begin(9, 10);
  radio.setPowerAmplificationLevel(NRF24_PA_MAX);
  radio.setDataRate(NRF24_250KBPS);
  radio.setActive(false);
  radio.stopListening();  
  delay(100);

  // turn on sensor
  pinMode(PIN_WATER_GND_A, OUTPUT);
  pinMode(PIN_WATER_GND_B, OUTPUT);
  digitalWrite(PIN_WATER_GND_A, LOW);
  digitalWrite(PIN_WATER_GND_B, LOW);
  delay(1000);
  
  long s = analogRead(PIN_ANALOG_A);
  double ds = 100.0 - 100.0 * (s - calMin) / (calMax - calMin);
  //if (ds < 0) {
  //  ds = 0;
  //} else if (ds > 100) {
  //  ds = 100;
  //}
  // turn off sensor
  pinMode(PIN_WATER_GND_A, INPUT);
  pinMode(PIN_WATER_GND_B, INPUT);

  // Turn on vin voltage divider
  pinMode(PIN_VIN_SENSE_GND, OUTPUT);
  digitalWrite(PIN_VIN_SENSE_GND, LOW);
  delay(1000);

  int nrvin = 100;
  unsigned long vins = 0;
  for (int i = 0; i < nrvin; i++) {
    vins += analogRead(PIN_ANALOG_VIN);
    delay(1);
  }
  int vinr = vins / nrvin; 

  double vin = (v*vinr/1024.0);
  if (true) { // false to prepare calibration, true to do it
    vin = vin / (2.4/2.33); // Calibration = read/actual
    vin = vin*2; // It was divided
  }

  pinMode(PIN_VIN_SENSE_GND, INPUT);
  // Turn off vin voltage divider

  int v1 = (int)v;
  int v2 = (int)((v-(int)v)*100);
  int vin1 = (int)vin;
  int vin2 = (int)((vin-(int)vin)*100);

  #ifdef DEBUG
  Serial.print("s = ");  Serial.print(s); Serial.println();
  Serial.print("VCC = ");  Serial.print(v); Serial.println(" Volts");
  Serial.print("v1 = ");  Serial.print(v1); Serial.println();
  Serial.print("v2 = ");  Serial.print(v2); Serial.println();
  Serial.print("VIN = ");  Serial.print(vin); Serial.println(" Volts");
  Serial.print("vin1 = ");  Serial.print(vin1); Serial.println();
  Serial.print("vin2 = ");  Serial.print(vin2); Serial.println();
  Serial.print("seq = ");  Serial.print(seq); Serial.println();
  #endif

  snprintf(msg, 32, "%04lX;%02X;%d.%02d;%d.%02d;%ld;%ld", addr&0xFFFF, seq, v1, v2, vin1, vin2, s, (long)ds);
  seq++;

  send(radio, msg);

  // Radio Off
  delay(100);
  pinMode(PIN_RADIO_GND_A, INPUT);
  pinMode(PIN_RADIO_GND_B, INPUT);

  if (vin < minVIN) {
    // Voltage too low. Sleep forever to prevent battery damage.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
  } else {
    sleep(interval);
  }
}

void sleep(unsigned long secs) {
  while (secs > 9) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
    secs -= 8;
  }
  if (secs > 5) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
    secs -= 4;
  }
  delay(1000*secs);
}

bool send(NRF24 radio, char* msg) {
  #ifdef DEBUG
  Serial.print("Sending: \""); Serial.print(msg); Serial.print("\" ");
  #endif

  radio.setRetries(15, 15);
  uint8_t numAttempts = -1;
  bool sent = radio.send(radioAddr, (uint8_t*)msg, strlen(msg)+1, &numAttempts);  // make sure the first parameter matches the receiver's address
  radio.setActive(false);
  
  #ifdef DEBUG
  if (sent) {
    Serial.print("OK");
  } else {
    Serial.print("failed");
  }
  Serial.print(" after ");
  Serial.print(numAttempts);
  Serial.println(" attempts.");
  #endif
  return sent;
}

