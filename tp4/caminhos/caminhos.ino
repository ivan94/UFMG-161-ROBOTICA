#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include "ShaftEncoder.hpp"
#include "MotorController.hpp"
#include "Menu.hpp"

/************ Menu Libraries ******************/
LiquidCrystal lcd(8,9,4,5,6,7);
Menu menu(&lcd);

/************ Motor Libraries ******************/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

ShaftEncoder encoderM1(A13, 4, 1);
MotorController controllerM1(1, AFMS, -1, &encoderM1);

ShaftEncoder encoderM2(A14, 4, 1);
MotorController controllerM2(2, AFMS, 1, &encoderM2);


/************ Infra vermelho ******************/
int RECV_PIN = 53;
IRrecv irrecv(RECV_PIN);
decode_results results;

int REMOTE_CODE = 0x20DF;

unsigned long IR_UP = 0x20DF02FD;
unsigned long IR_DOWN = 0x20DF827D;
unsigned long IR_LEFT = 0x20DFE01F;
unsigned long IR_RIGHT = 0x20DF609F;
unsigned long IR_POWER = 0x20DF10EF;
unsigned long IR_BACK = 0x20DF14EB;
unsigned long IR_OK = 0x20DF22DD;
unsigned long IR_1 = 0x20DF8877;
unsigned long IR_2 = 0x20DF48B7;
unsigned long IR_3 = 0x20DFC837;
unsigned long IR_4 = 0x20DF28D7;
unsigned long IR_5 = 0x20DFA857;
unsigned long IR_6 = 0x20DF6897;
unsigned long IR_7 = 0x20DFE817;
unsigned long IR_8 = 0x20DF18E7;
unsigned long IR_9 = 0x20DF9867;
unsigned long IR_0 = 0x20DF08F7;

/************ LOCAL VARS ******************/
const String LCD_BLANK = "                ";

int task = 0;
unsigned long baseTime = 0;
int shapeSize = 50;

void setup() {
  //BOOT LIBS
  lcd.begin(16, 2);
  AFMS.begin();
  menu.boot();
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);

  menu.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if((results.value >> 16) ==  REMOTE_CODE){
      unsigned long code = results.value;
      if(code == IR_DOWN){
        runTask(0);
        menu.next();
      }else if(code == IR_UP){
        runTask(0);
        menu.prev();
      }else if(code == IR_OK){
        menu.enter();
      }else if(code == IR_BACK){
        runTask(0);
        menu.back();
      }else if(code == IR_POWER){
        runTask(0);
      }else if(code == IR_LEFT){
        runTask(0);
      }else if(code == IR_RIGHT){
        runTask(0);
      }else{
        buttonPressed(code);
      }
      
      delay(300);
    }
    //Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }

  runTask(task);
  controllerM1.control();
  controllerM2.control();
}

//convert controller number to int
int btnToNum(unsigned long code){
  if(code == IR_0){
    return 0;
  }
  if(code == IR_1){
    return 1;
  }
  if(code == IR_2){
    return 2;
  }
  if(code == IR_3){
    return 3;
  }
  if(code == IR_4){
    return 4;
  }
  if(code == IR_5){
    return 5;
  }
  if(code == IR_6){
    return 6;
  }
  if(code == IR_7){
    return 7;
  }
  if(code == IR_8){
    return 8;
  }
  if(code == IR_9){
    return 9;
  }
}

const double ROT_SPEED_RATIO = 2.75;
const double VEHICLE_RADIUS = 10.5;

double spd = 2;

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

//when a number on the controller is pressed
void buttonPressed(unsigned long code){
  if(task == 14){
    shapeSize = shapeSize*10 + btnToNum(code);
    if(shapeSize >= 2000){
      shapeSize -= 2000;
    }
    if(shapeSize >= 1000){
      shapeSize = shapeSize - 1000;
    }
    if(shapeSize > 200){
      shapeSize = 200;
    }
  }
}

void runTask(int newTask){
  if(task != newTask){
    baseTime = millis();
    task = newTask;
  }
  if(task == 0){
    controllerM1.setGoal(0);
    controllerM2.setGoal(0);
  }else if(task == 14){
    lcd.setCursor(0, 1);
    lcd.print("Tamanho: ");
    lcd.print(shapeSize);
    lcd.print(LCD_BLANK);
  }else if(task == 11){
    double s = spd;
    unsigned long tempoCurva = angleToDelay(90, s);
    unsigned long tempoReta = distanceToDelay(shapeSize, s);
    if((millis()-baseTime) < tempoReta){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 2*tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 2*tempoReta + 2*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 3*tempoReta + 2*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 3*tempoReta + 3*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 4*tempoReta + 3*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 4*tempoReta + 4*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else{
      runTask(0); 
    }
  }else if(task == 12){
    double s = spd;
    unsigned long tempoCurva = angleToDelay(120, s);
    unsigned long tempoReta = distanceToDelay(shapeSize, s);
    if((millis()-baseTime) < tempoReta){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 2*tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 2*tempoReta + 2*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 3*tempoReta + 2*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 3*tempoReta + 3*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else{
      runTask(0);
    }
  }else if(task == 13){
    double s = spd;
    unsigned long tempoCurva = angleToDelay(180, s);
    unsigned long tempoReta = distanceToDelay(shapeSize, s);
    if((millis()-baseTime) < tempoReta){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else if((millis()-baseTime) < 2*tempoReta + tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(s);
    }else if((millis()-baseTime) < 2*tempoReta + 2*tempoCurva){
      controllerM1.setGoal(s);
      controllerM2.setGoal(-s);
    }else{
      runTask(0);
    }
  }
}

