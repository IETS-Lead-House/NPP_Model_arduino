#include <SoftwareSerial.h>
#include <GyverStepper.h>

#define M01 24
#define M11 26
#define M21 28
#define STEP1 30
#define DIR1 34

#define M02 6
#define M12 5
#define M22 4
#define STEP2 3
#define DIR2 2

#define M03 11
#define M13 10
#define M23 9
#define STEP3 8
#define DIR3 7

#define MAGNET 36

#define BLUETOOTH_IN 13
#define BLUETOOTH_OUT 12

#define DELAY 20 

#define DEATH_ZONE_XY 9
#define DEATH_ZONE_Z 4

// divided by 15 already
#define XMOTION_MAX 400/15 
#define YMOTION_MAX 400/15 
#define ZMOTION_MAX 100/15 


//17HS4401 1.5A Nema17
GStepper<STEPPER2WIRE> motorX(200*4, STEP1, DIR1);
GStepper<STEPPER2WIRE> motorY(200*4, STEP2, DIR2);
GStepper<STEPPER2WIRE> motorZ(200*16, STEP3, DIR3);


SoftwareSerial Bluetooth(BLUETOOTH_IN, BLUETOOTH_OUT);

byte bytes[6];

boolean valid = false;

void setup() {
  pinMode(BLUETOOTH_IN, INPUT);
  pinMode(BLUETOOTH_OUT, OUTPUT);
  
  //XY use channel 1/4 (010) Z use channel 1/16 (001)
  pinMode(M11, OUTPUT);
  digitalWrite(M11, HIGH);
  pinMode(M12, OUTPUT);
  digitalWrite(M12, HIGH);
  pinMode(M23, OUTPUT);
  digitalWrite(M23, HIGH);

  pinMode(MAGNET, OUTPUT);
  digitalWrite(MAGNET, LOW);
  
  initMotion();
  
  Serial.begin(9600);
  Bluetooth.begin(57600);
  //Serial.println("Reciever start");
}

void initMotion() {
  motorX.setRunMode(KEEP_SPEED);
  motorX.autoPower(true);
  motorX.setSpeed(25);
  motorY.setRunMode(KEEP_SPEED);
  motorY.autoPower(true);
  motorY.setSpeed(25);
  motorZ.setRunMode(KEEP_SPEED);
  motorZ.autoPower(true);
  motorZ.setSpeed(25);
  
  motorX.setSpeed(0);
  motorY.setSpeed(0);
  motorZ.setSpeed(0);
}

void loop() { 
  
  if(Bluetooth.available()) {
    recieveData();   
    if(valid) {
      int8_t 
      xAxis1 = bytes[0],
      yAxis1 = bytes[1],
      xAxis2 = bytes[2],
      yAxis2 = bytes[3];
    
      boolean 
      button1 = bytes[4] & 1,
      button2 = bytes[4] & 2,
      stick_button1 = bytes[4] & 4,
      stick_button2 = bytes[4] & 8;
      //logging(xAxis1, yAxis1, xAxis2, yAxis2, button1, button2, stick_button1, stick_button2);

      motionLogic(xAxis1, yAxis1, yAxis2);
      magnetLogic(button1, button2);
    }
  }
    motorX.tick();
    motorY.tick(); 
    motorZ.tick();
}

void motionLogic(int8_t xAxis1, int8_t yAxis1, int8_t yAxis2) {
  float dx = (xAxis1+yAxis1), dy = (xAxis1-yAxis1);
  if(dx*dx >= DEATH_ZONE_XY) { 
        // выставляем скорость
    if(motorX.getSpeed() * dx < 0) {
      motorX.brake();  
    }
    motorX.setSpeed(dx*XMOTION_MAX);
  } else {
    motorX.brake();
    motorX.setSpeed(0);
  }
  if(dy*dy >= DEATH_ZONE_XY) { 
        // выставляем скорость
    if(motorY.getSpeed() * dy < 0) {
      motorY.brake();  
    }
    motorY.setSpeed(dy*YMOTION_MAX);
  } else {
    motorY.brake();  
    motorY.setSpeed(0);
  }
  if(yAxis2 * yAxis2 >= DEATH_ZONE_Z) {
    if(motorZ.getSpeed() * yAxis2 < 0) {
      motorZ.brake();  
    }
    motorZ.setSpeed(yAxis2*ZMOTION_MAX);
  } else {
     motorZ.brake();
     motorZ.setSpeed(0);
  }
      //Serial.print("xm:");Serial.println(motorX.getSpeed());
      //Serial.print("ym:");Serial.println(motorY.getSpeed());
      //Serial.print("zm:");Serial.println(motorZ.getSpeed());
}

// если кнопка 1 активна, И не активна кнопка 2 - магнит активирован, иначе деактивирован
void magnetLogic(boolean button1, boolean button2) {
  if(button1) {
    digitalWrite(MAGNET, HIGH);
  }
  if(button2) {
    digitalWrite(MAGNET, LOW);
  }
}

void recieveData() { 
  int cache = 0;
  valid = false;
  for (int i = 0; i < 8; ++i) {
    while(!Bluetooth.available()) {
      delay(5);  
    }
    cache = Bluetooth.read();
    if(i == 0 && cache != 17) {
      return;
    }
    if(i == 1 && cache != 19) {
      return;
    }
    if(i > 1) {
      bytes[i - 2] = cache;
    }
  }
  if((bytes[0] + 7 * bytes[1] + 11 * bytes[2] + 13 * bytes[3] + 17 * bytes[4]) % 255 == bytes[5]) {
    valid = true;
    return;
  }
  return;
}

void logging(int8_t x1, int8_t y1, int8_t x2, int8_t y2, boolean btn1, boolean btn2, boolean s_btn1, boolean s_btn2) {
  Serial.print("OUT: "); 
  Serial.print("[ x1 : "); 
  Serial.print(x1); 
  Serial.print(" y1 : ");  
  Serial.print(y1); 
  Serial.print(" s_btn1 : "); 
  Serial.print(s_btn1);  
  Serial.print(" x2 : ");  
  Serial.print(x2); 
  Serial.print(" y2 : ");  
  Serial.print(y2); 
  Serial.print(" s_btn2 : ");
  Serial.print(s_btn2); 
  Serial.print(" btn1 : ");     
  Serial.print(btn1);
  Serial.print(" btn2 : ");     
  Serial.print(btn2); 
  Serial.println(" ]");  
}
