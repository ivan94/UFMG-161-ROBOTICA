#include <Arduino.h>
#include <LiquidCrystal.h>
#include "ColorDetect.hpp"

#define tolerancia 50

extern LiquidCrystal lcd;

ColorDetector::ColorDetector(int ldr, int red, int green, int blue){
  this->ldr = ldr;
  this->red = red;
  this->green = green;
  this->blue = blue;
  
  for(int i = 0; i<3; i++){
    colourArray[i] = 0;
    whiteArray[i] = 0;
    blackArray[i] = 0;
  }

  ledArray[0] = red;
  ledArray[1] = green;
  ledArray[2] = blue;
  

  ValorLidoR = 0;
  ValorLidoG = 0;
  ValorLidoB = 0;

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}


void ColorDetector::calibrate(){
  lcd.setCursor(0, 1);
  lcd.print("Calib. Branco      ");
  //set white balance
   delay(5000);                              //delay for five seconds, this gives us time to get a white sample in front of our sensor
  //scan the white sample.
  //go through each light, get a reading, set the base reading for each colour red, green, and blue to the white array
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(100);
     getReading(5);          //number is the number of scans to take for average, this whole function is redundant, one reading works just as well.
     whiteArray[i] = avgRead;
     digitalWrite(ledArray[i],LOW);
     delay(100);
  }
  //done scanning white, now it will pulse blue to tell you that it is time for the black (or grey) sample.
   //set black balance
  lcd.setCursor(0, 1);
  lcd.print("Calib. Preto      ");
  delay(5000);              //wait for five seconds so we can position our black sample 
  //go ahead and scan, sets the colour values for red, green, and blue when exposed to black
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(100);
     getReading(5);
     blackArray[i] = avgRead;
     //blackArray[i] = analogRead(2);
     digitalWrite(ledArray[i],LOW);
     delay(100);
  }
  lcd.setCursor(0, 1);
  lcd.print("Calibrado            ");
}

void ColorDetector::checkColour(){
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);  //turn or the LED, red, green or blue depending which iteration
     delay(100);                      //delay to allow CdS to stabalize, they are slow
     getReading(5);                  //take a reading however many times
     colourArray[i] = avgRead;        //set the current colour in the array to the average reading
     float greyDiff = whiteArray[i] - blackArray[i];                    //the highest possible return minus the lowest returns the area for values in between
     colourArray[i] = (colourArray[i] - blackArray[i])/(greyDiff)*255; //the reading returned minus the lowest value divided by the possible range multiplied by 255 will give us a value roughly between 0-255 representing the value for the current reflectivity(for the colour it is exposed to) of what is being scanned
     digitalWrite(ledArray[i],LOW);   //turn off the current LED
     delay(100);
  }
}

void ColorDetector::getReading(int times){
  int reading;
  int tally=0;
  //take the reading however many times was requested and add them up
  for(int i = 0;i < times;i++){
    reading = analogRead(ldr);
    tally = reading + tally;
    delay(10);
  }
  //calculate the average and set it
  avgRead = (tally)/times;
}

//0 Preto
//1 Vermelho
//2 Verde
//3 Azul
int ColorDetector::getColour(){
  ValorLidoR = colourArray[0];
  ValorLidoG = colourArray[1];
  ValorLidoB = colourArray[2];

  /*if( ValorLidoR< branco(0,1) && ValorLidoG <branco(1,1) && ValorLidoB <branco(2,1) 
  && ValorLidoR>branco(0,-1) && ValorLidoG > branco(1,-1) && ValorLidoB >branco(2,-1) ){
    //Serial.println("Branco");
  }

  else if( ValorLidoR< tolerancia && ValorLidoG <tolerancia && ValorLidoB <tolerancia 
  && ValorLidoR>(-1*tolerancia) && ValorLidoG > (-1*tolerancia) && ValorLidoB > (-1*tolerancia) ){
   // Serial.println("Preto");
  }
  else if (ValorLidoB < ValorLidoR && ValorLidoB < ValorLidoG  && ValorLidoG < ValorLidoR && ValorLidoR < (ValorLidoG+tolerancia) && ValorLidoR > (ValorLidoG-tolerancia)) {  
     Serial.println("Amarelo");
  } 
  
  else if (ValorLidoG < ValorLidoR && ValorLidoB < ValorLidoR && ValorLidoB < ValorLidoG) {  
    Serial.println("Vermelho"); 
  } 
  //Verifica se a cor verde foi detectada  
  else if (ValorLidoR < ValorLidoG && ValorLidoB < ValorLidoG) {  
    Serial.println("Verde");
  }  
  //Verifica se a cor azul foi detectada  
  else if (ValorLidoR < ValorLidoB && ValorLidoG < ValorLidoB && ValorLidoR < ValorLidoG ) {  
     Serial.println("Azul");
  }
  else{
     Serial.println("Num eh nada");
  }*/
    
  if (ValorLidoB < ValorLidoR && ValorLidoB < ValorLidoG  && ValorLidoG < ValorLidoR && ValorLidoR < (ValorLidoG+tolerancia) && ValorLidoR > (ValorLidoG-tolerancia)) {  
    return 0;
  }
  
  else if (ValorLidoG < ValorLidoR && ValorLidoB < ValorLidoR) {  
    return 1;
  } 
  //Verifica se a cor verde foi detectada  
  else if (ValorLidoR < ValorLidoG && ValorLidoB < ValorLidoG) {  
    return 2;
  }  
  //Verifica se a cor azul foi detectada  
  else if (ValorLidoR < ValorLidoB && ValorLidoG < ValorLidoB) {  
     return 3;
  }
  
}



