#ifndef __MOTORCONTROLLER_H
#define __MOTORCONTROLLER_H

#include <Adafruit_MotorShield.h>
#include <PID_v1.h>
#include "ShaftEncoder.hpp"

class MotorController {
private:
  PID controller;
  unsigned long baseTime;
  double goalSpeed;

  double avgSpeed;
  int counter = 0;

  double value;
  double y;
  double speed;
  int dirCorrect;
  //SHAFT ENCODER;

  Adafruit_DCMotor *motor;
  ShaftEncoder* encoder;

  void applySpeed();
public:
  MotorController (int motorId, Adafruit_MotorShield& shield, int dirCorrect, ShaftEncoder* encoder);
  // virtual ~MotorController ();

  void setGoal(double goalSpeed);
  void control();
};

#endif
