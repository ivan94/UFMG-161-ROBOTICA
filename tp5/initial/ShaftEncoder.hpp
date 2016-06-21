#ifndef __SHAFTENCODER_H
#define __SHAFTENCODER_H

class ShaftEncoder {
private:
  int bbId;

  unsigned long counter;
  unsigned long ct;
  unsigned long baseTime;
  unsigned long timeOfRotation;

  //setps per rotation
  int spr;
  int rotation;
  double wheelPerimeter;
  unsigned long spd;

  bool state;

  long lastDebounceTime = 0;  // the last time the output pin was toggled
  long debounceDelay = 10;    // the debounce time; increase if the output flickers

  bool init;

  int analogToDigital(int analog);
public:
  ShaftEncoder (int bbId, int spr, double wheelPerimeter);
  // virtual ~ShaftEncoder ();

  double getDistance();
  unsigned long getCounter();
  double getSpeed();

  void boot();
  void read();

};

#endif
