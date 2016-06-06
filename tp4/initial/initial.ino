#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "MotorController.hpp"
#include "ShaftEncoder.hpp"


/************ LCD Shield buttons ******************/

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/


 return btnNONE;  // when all others fail, return this...
}

LiquidCrystal lcd(8,9,4,5,6,7); 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

ShaftEncoder encoderM1(A13, 4, 1);
MotorController controllerM1(1, AFMS, -1, &encoderM1);

ShaftEncoder encoderM2(A14, 4, 1);
MotorController controllerM2(2, AFMS, 1, &encoderM2);

double speed = 0;
double goalSpeed = 30;
double value = 0;

bool begin = false;
unsigned long base;

long retaTempo = 8000;
long curvaTempo = 6200;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AFMS.begin();

  lcd.begin(16, 2);
}

void loop() {
  int button = read_LCD_buttons();
  if(button == btnUP){
    begin = true;
    base = millis();
  }
  if(button == btnRIGHT){
    //controllerM1.setGoal(2);
    //controllerM2.setGoal(-2);
  }
  if(button == btnDOWN){
    begin = false;
  }


  if(begin){
    if((millis()-base) < retaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(1);
    }else if((millis()-base) < retaTempo+curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(-1);
    }else if((millis()-base) < 2*retaTempo+curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(1);
    }else if((millis()-base) < 2*retaTempo+2*curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(-1);
    }else if((millis()-base) < 3*retaTempo+2*curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(1);
    }else if((millis()-base) < 3*retaTempo+3*curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(-1);
    }else if((millis()-base) < 4*retaTempo+3*curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(1);
    }else if((millis()-base) < 4*retaTempo+4*curvaTempo){
      controllerM1.setGoal(1);
      controllerM2.setGoal(-1);
    }else{
      controllerM1.setGoal(0);
      controllerM2.setGoal(0);
    }
  }else{
    controllerM1.setGoal(0);
    controllerM2.setGoal(0);
  }

  lcd.setCursor(0, 0);
  //lcd.print(millis()-base);
  //lcd.print(" - ");
  //lcd.print(encoderM1.getDistance());
  //lcd.print("               ");
  controllerM1.control();
  lcd.setCursor(0, 1);
  controllerM2.control();
  //lcd.print(encoderM2.getSpeed());
  //lcd.print(" - ");
  //lcd.print(encoderM2.getDistance());
  //lcd.print("               ");
}
