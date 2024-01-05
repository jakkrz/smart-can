#include <SoftwareSerial.h>
#include <Servo.h>

namespace pins {
  constexpr int servoA = 6;
  constexpr int servoB = 7;
  
  constexpr int motorIN1 = 3;
  constexpr int motorIN2 = 2;
  constexpr int motorIN3 = 4;
  constexpr int motorIN4 = 5;

  constexpr int sensorEcho = 9;
  constexpr int sensorTrigger = 8;

  //constexpr int bluetoothReceive = 6;
  //constexpr int bluetoothTransmit = 7;
}

constexpr int openForMilliseconds = 1000; // how long the lid is open for after opening
constexpr int maximumOpeningDistance = 30;

int distanceHits = 0;
constexpr int requiredDistanceHits = 100;
constexpr int decreaseDistanceHitsOnMissAmount = 5;

Servo servoA;
Servo servoB;

//SoftwareSerial bluetoothSerial(pins::bluetoothReceive, pins::bluetoothTransmit);
int lastDetected = -1;


void openLid() {
  servoA.write(135);
  servoB.write(135);  
}

void closeLid() {
  servoA.write(180);
  servoB.write(0);  
}

void handleLid() {
  int now = millis();
  int timeSinceDetected = now - lastDetected;
  
  if (lastDetected != -1 && timeSinceDetected <= openForMilliseconds) {
    openLid();
  } else {
    lastDetected = -1;
    closeLid();  
  }
}

void handleBluetooth() {
  if (Serial.available() > 0) {
    char message = Serial.read();

    switch (message) {
      case '0':
        stopMoving();
        break;
      case '1':
        driveForward();
        break;
      case '2':
        driveBackward();
        break; 
      case '3':
        turnRight();
        break;
      case '4':
        turnLeft();
        break;
      case '5':
        lastDetected = millis();
        break;
    }
  }
}

void driveForward() {
  digitalWrite(pins::motorIN1, HIGH);
  digitalWrite(pins::motorIN2, LOW);
  digitalWrite(pins::motorIN3, HIGH);
  digitalWrite(pins::motorIN4, LOW);
}

void driveBackward() {
  digitalWrite(pins::motorIN1, LOW);
  digitalWrite(pins::motorIN2, HIGH);
  digitalWrite(pins::motorIN3, LOW);
  digitalWrite(pins::motorIN4, HIGH);
}

void stopMoving() {
  digitalWrite(pins::motorIN1, LOW);
  digitalWrite(pins::motorIN2, LOW);
  digitalWrite(pins::motorIN3, LOW);
  digitalWrite(pins::motorIN4, LOW);
}

void turnLeft() {
  digitalWrite(pins::motorIN1, HIGH);
  digitalWrite(pins::motorIN2, LOW);
  digitalWrite(pins::motorIN3, LOW);
  digitalWrite(pins::motorIN4, HIGH);
}

void turnRight() {
  digitalWrite(pins::motorIN1, LOW);
  digitalWrite(pins::motorIN2, HIGH);
  digitalWrite(pins::motorIN3, HIGH);
  digitalWrite(pins::motorIN4, LOW);
}

int getSensorDistance() {
  digitalWrite(pins::sensorTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pins::sensorTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pins::sensorTrigger, LOW);
  
  long duration = pulseIn(pins::sensorEcho, HIGH);
  
  return duration * 0.034 / 2;
}

void handleSensing() {
  int distance = getSensorDistance();

  if (distance != 0 && distance <= maximumOpeningDistance) {
    distanceHits += 1;

    if (distanceHits >= requiredDistanceHits) {
      distanceHits = 0;
      lastDetected = millis();
    }
  } else {
    distanceHits = max(distanceHits - decreaseDistanceHitsOnMissAmount, 0);
  }
}

void setup() {
  servoA.attach(pins::servoA);
  servoB.attach(pins::servoB);
  //bluetoothSerial.begin(9600);
  Serial.begin(9600);

  pinMode(pins::motorIN1, OUTPUT);
  pinMode(pins::motorIN2, OUTPUT);
  pinMode(pins::motorIN3, OUTPUT);
  pinMode(pins::motorIN4, OUTPUT);

  pinMode(pins::sensorEcho, INPUT);
  pinMode(pins::sensorTrigger, OUTPUT);

}

void loop() {
  handleLid();
  handleBluetooth();
  handleSensing();
}
