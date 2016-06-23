#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "MotorController.hpp"

extern LiquidCrystal lcd;

#define COMPUTE_DELAY 200

MotorController::MotorController(int motorId, Adafruit_MotorShield& shield, int dirCorrect, ShaftEncoder* encoder):controller(&speed, &y, &goalSpeed, 23, 0, 0.5, DIRECT){
  goalSpeed = 0;
  avgSpeed = 0;
  this->motorId = motorId;
  motor = shield.getMotor(motorId);
  this->dirCorrect = dirCorrect;
  this->encoder = encoder;
  controller.SetMode(AUTOMATIC);
  controller.SetOutputLimits(-150, 150);
  controller.SetSampleTime(COMPUTE_DELAY);
  baseTime = millis();
  value = 0;
  encoder->boot();
}

void MotorController::setGoal(double speed){
  if(speed != goalSpeed){
    goalSpeed = speed;
    encoder->boot();
    value = 0;
  }
}

void MotorController::control(){
  encoder->read();
  if(goalSpeed == 0){
    value = 0;
    applySpeed();
    return;
  }
  if((millis() - baseTime) < COMPUTE_DELAY){
    return;
  }
  //GET speed using senso
  speed = encoder->getSpeed();
  if(value < 0){
    speed = -speed;
  }
  controller.Compute();
//  lcd.print(speed);
//  lcd.print(" - ");
//  lcd.print(value);
//  lcd.print("                ");


  value += y;
  if(value > 255){
    value = 255;
  }
  if(value < -255){
    value = -255;
  }

  applySpeed();

  baseTime = millis();
}

void MotorController::applySpeed(){
  double v = value*dirCorrect;
  motor->setSpeed(abs(v));
  if(v == 0){
    motor->run(RELEASE);
  }else if(v>0){
    motor->run(FORWARD);
  }else{
    motor->run(BACKWARD);
  }
}
