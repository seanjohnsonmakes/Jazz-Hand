#include <PowerNote.h>  // Custom library written for this project
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();  // Adafruint LSM9DS1 sensor

#define LSM9DS1_SCK A5
#define LSM9DS1_MOSI A4

void setup() {
  // LED light pins
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(115200);
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
}

// Instantiating the power notes for each finger
// Parameters are (name, analog input pin, led pin, threshold, note, velocity, channel)
// We are using the first five notes of the A minor scale
PowerNote pinky("Pinky", A9, 9, 600, 50, 100, 1);
PowerNote ring("Ring", A8, 8, 625, 52, 100, 1);
PowerNote middle("Middle", A7, 5, 760, 53, 100, 1);
PowerNote pointer("Pointer", A6, 10, 585, 55, 100, 1);
PowerNote thumb("Thumb", A2, 11, 560, 57, 100, 1);

// Instantiating timer (for debouncing)
elapsedMillis ms = 0;

// Alpha values for EWMA on the acceleration input
float zAlpha = 0.4;
float xAlpha = 0.2;

// Instantiating acceleration EWMAs
double zAccelAVG = 0;
double xAccelAVG = 0;

void loop() {
  // Plays a note for each curled finger
  pinky.notePlayer();
  ring.notePlayer();
  middle.notePlayer();
  pointer.notePlayer();
  thumb.notePlayer();

  // Acquiring sensor data
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);

  // Timing the acceleration input smoothes things out
  if (ms >= 20) {
    ms = 0;
    // Further smoothing and response adjustment done with exponentially weighted moving average
    zAccelAVG = zAlpha * rangeMap(0, 20, 0, 127, a.acceleration.z) + (1-zAlpha) * zAccelAVG;
    xAccelAVG = xAlpha * rangeMap(0, 15, 0, 127, a.acceleration.x) + (1-xAlpha) * xAccelAVG;
    usbMIDI.sendControlChange(11, zAccelAVG, 1);  // 11 = expression control
    usbMIDI.sendControlChange(1, xAccelAVG, 1); // 1 = modulation control
  }
  
  Serial.println(" ");
}

// Linearly maps a number betweeen a and b to a number between c and d
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



