#include <PowerNote.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MOSI A4


void setupSensor() {
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");
  setupSensor();
  digitalWrite(13, HIGH);
}

PowerNote pinky("Pinky", A9, 3, 650, 21, 127, 1);
PowerNote ring("Ring", A8, 4, 650, 19, 127, 1);
PowerNote middle("Middle", A7, 5, 760, 17, 127, 1);
PowerNote pointer("Pointer", A6, 6, 600, 16, 127, 1);
PowerNote thumb("Thumb", A2, 7, 550, 14, 127, 1);

elapsedMillis ms = 0;

void loop() {
  pinky.notePlayer();
  ring.notePlayer();
  middle.notePlayer();
  pointer.notePlayer();
  thumb.notePlayer();

  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);

  
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("Y: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("Z: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");

  if (ms >= 20) {
    ms = 0;
    usbMIDI.sendControlChange(11, rangeMap(0, 40, 0, 127, abs(a.acceleration.z)), 1);
  }
  
  Serial.println(" ");
}

double rangeMap(float a, float b, float c, float d, float input) {
  // Takes an input between a and b and linearly maps the output between c and d
  float x = 0;
  if (input > b) {
    x = b;
  } else if (input < a) {
    x = a;
  } else {
    x = input;
  }
  double value = (c * (1 - ((x - a) / (b - a)))) + (d * ((x - a) / (b - a)));
  return value;
}



