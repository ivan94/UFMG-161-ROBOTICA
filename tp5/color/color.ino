#include <Stepper.h>

#include "ColorDetect.hpp"

const int stepsPerRevolution = 500;
Stepper myStepper(stepsPerRevolution, 4,6,5, 7);

/************ COLOR DETECOT LIB ******************/
int LDR_PIN = A4;

int BLOCK_TRESHOLD = 600;

ColorDetector detector(LDR_PIN, 10, 9, 8);

int BUZZER_PIN = 45;

int detecting_state = 0;

void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(60);
  detector.calibrate();
  Serial.begin(9600);
}

void loop() {
  if(analogRead(LDR_PIN) > 300){
    detector.checkColour();
    delay(100);
    Serial.println(detector.getColour());
    int c = detector.getColour();
    if(c == 0){
      myStepper.step(1024);
    }
  }

}
