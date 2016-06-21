#include <Arduino.h>
#include "ShaftEncoder.hpp"

ShaftEncoder::ShaftEncoder(int bbId, int spr, double wheelPerimeter){
  this->bbId = bbId;
  this->spr = spr;
  this->wheelPerimeter = wheelPerimeter;
  init = true;
}

void ShaftEncoder::boot(){
  baseTime = millis();
  counter = 0;
  ct = 0;
  spd = 0;
  rotation = 0;
  timeOfRotation = INFINITY;
  state = analogToDigital(analogRead(bbId));
  init = true;
}

void ShaftEncoder::read(){
  int s = analogToDigital(analogRead(bbId));
  if(s == LOW && state == HIGH){
    counter++;
    ct++;
    if(counter % spr == 0){
        rotation++;
    }
    timeOfRotation = millis()-baseTime;
    spd += timeOfRotation;
    baseTime = millis();
    
    init = false;
  }
  state = s;
}

unsigned long ShaftEncoder::getCounter(){
  return this->counter;
}

double ShaftEncoder::getDistance(){
  return rotation * wheelPerimeter;
}

double ShaftEncoder::getSpeed(){
  if(init){
    return 0;
  }
  if((millis()-baseTime) > 800){
    return 0; 
  }
  //unsigned long t = timeOfRotation / 10; 
  //timeOfRotation = spd / (double) ct;
  spd = 0;
  ct = 0;
  return wheelPerimeter/(timeOfRotation*spr/1000.0);
}

int ShaftEncoder::analogToDigital(int analog){
  if(state == HIGH && analog <= 800){
    return LOW;
  }else if(state == LOW && analog >= 800){
    return HIGH;
  }else{
    return state;
  }
}

