#ifndef __COLOR_DETECT_H
#define __COLOR_DETECT_H

class ColorDetector{
private:
  int ldr;
  int red;
  int green;
  int blue;

  int ValorLidoR;
  int ValorLidoG;
  int ValorLidoB;

  int ledArray[3];

  float colourArray[3];
  float whiteArray[3];
  float blackArray[3];
  float redArray[3];
  float blueArray[3];
  float greenArray[3];
  float yellowArray[3];

  //place holder for average
  int avgRead;
public:
  ColorDetector(int ldr, int red, int green, int blue);

  
  void calibrate();
  void checkColour();
  void getReading(int times);
  double colorDistance(float* color, float* reference);
  int getColour();
};

/*avoid setupDetector();
void readColorValues();
String getColorName();*/

#endif
