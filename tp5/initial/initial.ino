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

Adafruit_DCMotor* motor = AFMS.getMotor(1);

ShaftEncoder encoderM1(A14, 6, 1);
MotorController controllerM1(1, AFMS, 1, &encoderM1);

ShaftEncoder encoderM2(A15, 6, 1);
MotorController controllerM2(2, AFMS, -1, &encoderM2);

const double ROT_SPEED_RATIO = 2.75;
const double VEHICLE_RADIUS = 11;

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
    //runTask(1);
    motor->setSpeed(50);
    motor->run(FORWARD);
    delay(300);
  }else if(button == btnDOWN){
    //runTask(0);
    motor->run(RELEASE);
    delay(300);
  }

  runTask(task);

  //Serial.println(analogRead(A15));
  
  lcd.setCursor(0, 0);
  encoderM1.read();
  spd = encoderM1.getSpeed();
  if(millis() - baseTime >300){
    lcd.print(spd);
    lcd.print(LCD_BLANK);
    baseTime = millis();
  }
  //controllerM1.control();
  lcd.setCursor(0, 1);
  //controllerM2.control();
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

void runTask(int newTask){
  if(task != newTask){
    baseTime = millis();
    task = newTask;
  }
  if(task == 0){
    controllerM1.setGoal(0);
    controllerM2.setGoal(0);
  }else if(task == 1){
//    if((millis() - baseTime)%7000 < 5000){
//      controllerM1.setGoal(1);
//      controllerM2.setGoal(1);
//    }else if((millis() - baseTime)%7000 < 10000){
//      controllerM1.setGoal(-1);
//      controllerM2.setGoal(-1);
//    }
    controllerM1.setGoal(7);
    controllerM2.setGoal(7);
  }
}

