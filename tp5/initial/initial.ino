#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include "ColorDetect.hpp"
#include "ShaftEncoder.hpp"
#include "MotorController.hpp"
#include "Menu.hpp"

/************ Menu Libraries ******************/
LiquidCrystal lcd(8,9,4,5,6,7);
Menu menu(&lcd);

/************ COLOR DETECOT LIB ******************/
int LDR_PIN = A15;

int BLOCK_TRESHOLD = 600;

ColorDetector detector(LDR_PIN, 23, 25, 27);

int BUZZER_PIN = 45;

int detecting_state = 0;

/************ Motor Libraries ******************/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor* motor1 = AFMS.getMotor(1);
Adafruit_DCMotor* motor2 = AFMS.getMotor(2);

ShaftEncoder encoderM1(A14, 6, 1);
MotorController controllerM1(1, AFMS, 1, &encoderM1);

ShaftEncoder encoderM2(A15, 6, 1);
MotorController controllerM2(2, AFMS, -1, &encoderM2);

const double ROT_SPEED_RATIO = 5.11;
const double VEHICLE_RADIUS = 12;

/**********************SEGUE LINHA ********************/
int OR_left = 43;
int OR_right = 41;

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

/************ LOCAL VARS ******************/
const String LCD_BLANK = "                ";

int task = 0;
unsigned long baseTime = 0;


void setup() {
  //BOOT LIBS
  lcd.begin(16, 2);
  AFMS.begin();
  menu.boot();
//  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  baseTime = millis();
  menu.display();
}
double spd = 0;
void loop() {

  int button = read_LCD_buttons();

  if(button == btnUP){
    runTask(1);
//    /motor->setSpeed(50);
//    motor->run(FORWARD);
    delay(300);
  }else if(button == btnDOWN){
    runTask(0);
    //motor->run(RELEASE);
    delay(300);
  }else if(button == btnRIGHT){
    runTask(30);
    delay(300);
  }

  runTask(task);

  //Serial.println(analogRead(A15));
  
  lcd.setCursor(0, 0);
//  encoderM2.read();
//  spd = encoderM2.getSpeed();
//  if(millis() - baseTime >300){
//    lcd.print(spd);
//    lcd.print(LCD_BLANK);
//    baseTime = millis();
//  }
  controllerM1.control();
  lcd.setCursor(0, 1);
  controllerM2.control();
  //delay(100);
}

unsigned long distanceToDelay(double d, double s){
  //velocidade do carrinho
  double real_speed = ROT_SPEED_RATIO*s;
  //tempo para percorrer a distancia
  return (unsigned long) 1000*d/real_speed;
}

unsigned long angleToDelay(double a, double s){
  //distancia a ser percorrida para virar o angulo desejado
  double distance = 2*3.14*VEHICLE_RADIUS*(a/360);

  return distanceToDelay(distance, s);
}

double oldM = 0;
int oldD = 0;

int ct = 0;
long v = 0;

void runTask(int newTask){
  if(task != newTask){
    baseTime = millis();
    task = newTask;
  }
  if(task == 0){
    controllerM1.setGoal(0);
    controllerM2.setGoal(0);
  }else if(task == 1){
    if(analogRead(A8) < 150){
      runTask(2);
    }else{
      controllerM1.setGoal(0);
      controllerM2.setGoal(0);
    }
  }else if(task == 2){//orientaćão
    if(millis()-baseTime > 200){
      
      baseTime = millis();
      
      int measure = v/ct;
      v=0;
      ct=0;  
      
      lcd.setCursor(0, 0);
      lcd.print(measure);
      lcd.print(LCD_BLANK);
      lcd.setCursor(0, 1);
      lcd.print(oldM);
      lcd.print(LCD_BLANK);
      
      if(measure > oldM*1.03 && measure < oldM*0.97){
        lcd.setCursor(10, 0);
        lcd.print("ok");
        controllerM1.setGoal(0);
        controllerM2.setGoal(0);
        runTask(3);
        lcd.setCursor(10, 0);
        lcd.print("ok");
        //seguir reto ate chegar a linha preta
      }
    }else{
      v+= analogRead(A10) - analogRead(A9);
      ct++;
      controllerM1.setGoal(2);
      controllerM2.setGoal(-2);
    }
  }else if(task == 3){//forća a barra
    if(millis()-baseTime < 3000){
      //BY GABI
      motor1->setSpeed(190);
      motor2->setSpeed(190);
      motor1->run(BACKWARD);
      motor2->run(FORWARD);
    }else if(millis()-baseTime < 4500){
      motor1->setSpeed(190);
      motor2->setSpeed(190);
      motor1->run(BACKWARD);
      motor2->run(RELEASE);
    }else if(millis()-baseTime < 6000){
      motor1->setSpeed(190);
      motor2->setSpeed(190);
      motor1->run(RELEASE);
      motor2->run(FORWARD);
    }
    else {
      runTask(4);
    }
  }else if(task == 4){//faz caminho
    double t90 = angleToDelay(90, 2);
    double t1 = distanceToDelay(90, 2);
    double t2 = distanceToDelay(85, 2);
    double t3 = distanceToDelay(10, 2);
    if(millis()-baseTime < t1){
      controllerM1.setGoal(2);
      controllerM2.setGoal(2);
    }else if(millis()-baseTime < t1 + t90){
      controllerM1.setGoal(-2);
      controllerM2.setGoal(2);
    }else if(millis()-baseTime < t1 + t90 + (t2/2)){
      controllerM1.setGoal(2);
      controllerM2.setGoal(2);
    }else if(millis()-baseTime < t1 + t90 + (t2/2) + t90){
      controllerM1.setGoal(-2);
      controllerM2.setGoal(2);
    }else if(millis()-baseTime < t1 + t90 + (t2/2) + t90 + t1 + t3){
      controllerM1.setGoal(2);
      controllerM2.setGoal(2);
    }
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(-2);
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90 + t2){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(2);
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90 + t2 + 2*t90){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(-2);
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90 + t2 + 2*t90 + (t2/2)){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(2);
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90 + t2 + 2*t90 + (t2/2) + t90){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(-2);
//    }else if(millis()-baseTime < t1 + t90 + (t2/2) + 2*t90 + t2 + 2*t90 + (t2/2) + t90 + t1){
//      controllerM1.setGoal(2);
//      controllerM2.setGoal(2);
//    }
    else{
      runTask(0);
    }
  }
  else if(task == 30){
    //delay(1000);
    long v = 0;
    for(int i = 0; i<20; i++){
      v+= analogRead(A10) - analogRead(A9);
      delay(50);
    }
    oldM = v/20.0;
    lcd.setCursor(0, 0);
    lcd.print(oldM);
    lcd.print(LCD_BLANK);
    runTask(0);
  }
}


