#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing MAX30102...");

  // Initialize I2C
  Wire.begin();

  if (!particleSensor.begin(Wire, 100000)) {
    Serial.println("MAX30102 not found. Check wiring!");
    while (1);
  }

  Serial.println("Sensor found!");

  // MAX30102 configuration
  particleSensor.setup(
      0x1F,   // powerLevel: 0x1F=31 (~10mA), 0x3F=63 (~20mA)
      4,      // sampleAverage: good smoothing
      2,      // ledMode: 2 = red + IR
      100,    // sampleRate: 100Hz
      411,    // pulseWidth: 411us
      2048    // adcRange: 2048nA
  );
  particleSensor.setPulseAmplitudeRed(0x09);   // Was probably 0x3F or 0x7F
  particleSensor.setPulseAmplitudeIR(0x09);

  particleSensor.enableDIETEMPRDY();
  Serial.println("Place your finger on the sensor...");
}

// Variables for heart rate algorithm
const byte RATE_SIZE = 4; // smoothing
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0; // to track beat-to-beat timing

float beatsPerMinute;
int beatAvg;

void loop() {
  long irValue = particleSensor.getIR();

  if (irValue < 50000) {
    Serial.println("No finger detected");
    return;
  }

  // Detect heartbeat
  if (checkForBeat(irValue)) {
    long now = millis();
    long delta = now - lastBeat;
    lastBeat = now;

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print("  BPM: ");
  Serial.print(beatsPerMinute);
  Serial.print("  Avg BPM: ");
  Serial.println(beatAvg);

  delay(10);
}
