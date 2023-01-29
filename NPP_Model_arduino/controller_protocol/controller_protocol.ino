#include <SoftwareSerial.h>

#define STICK1_PWR 50
#define STICK2_PWR 51
#define BUTTON1_PWR 46
#define BUTTON2_PWR 48
//BLUETOOTH
#define BLUETOOTH_IN 18
#define BLUETOOTH_OUT 19

//LEFT BUTTON
#define BUTTON1 47

//RIGHT BUTTON
#define BUTTON2 49

//LEFT STICK
#define STICK1_BUTTON 52
#define STICK1_X A8
#define STICK1_Y A9

//RIGHT STICK
#define STICK2_BUTTON 53
#define STICK2_X A10
#define STICK2_Y A11

#define DELAY 20

SoftwareSerial Bluetooth(BLUETOOTH_IN, BLUETOOTH_OUT);

byte bytes[6];

void setup() {  
  pinMode(BLUETOOTH_IN, INPUT);
  pinMode(BLUETOOTH_OUT, OUTPUT);

  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(STICK1_BUTTON, INPUT_PULLUP);
  pinMode(STICK2_BUTTON, INPUT_PULLUP);
  // так решил электрик
  pinMode(STICK1_PWR, OUTPUT);
  digitalWrite(STICK1_PWR, HIGH);
  pinMode(STICK2_PWR, OUTPUT);
  digitalWrite(STICK2_PWR, HIGH);
  pinMode(BUTTON1_PWR, OUTPUT);
  digitalWrite(BUTTON1_PWR, HIGH);
  pinMode(BUTTON2_PWR, OUTPUT);
  digitalWrite(BUTTON2_PWR, HIGH);

  pinMode(STICK1_X, INPUT);
  pinMode(STICK1_Y, INPUT);
  pinMode(STICK2_X, INPUT);
  pinMode(STICK2_Y, INPUT);
  
  Serial.begin(9600);
  Bluetooth.begin(57600);
  Serial.println("Controller start");
}

String sendNormalString(String data) {
    Serial.print("SYS send: ");
    Serial.println(data);
    Bluetooth.print(data);  
    return data;  
}

String sendHashString(String data) {  
    int hash = hashString(data);
    Serial.print("SYS hash: ");
    Serial.println(hash);
    Serial.print("SYS send: ");
    Serial.println(data);
    for (int i = 0; i < 4; ++i) {
      Bluetooth.write(( hash >> 8*i ) & 0xFF);
    }
    
    Bluetooth.print(data);  
    return data;  
}

void onErr() {
  Serial.print("-error-");
  delay(10);  
  Bluetooth.flush();
}

void loop() {
  if(Serial.available()) {
    sendNormalString(Serial.readString());
  }
  if(Bluetooth.available()) {
    sendNormalString(Bluetooth.readString());
  }
  // Система
  //Serial.print("SYS: ");
  //sendData();
  // Приемник
}

int hashString(String s) {
  int hash = 0;
  for(int i = 0; i < s.length(); ++i) {
    hash *= 7;
    hash += s[i];
  }
  return hash;
}

void sendData() {
  int8_t 
  xAxis1 = -stickDeflection(STICK1_Y),
  yAxis1 = stickDeflection(STICK1_X),
  xAxis2 = stickDeflection(STICK2_Y),
  yAxis2 = -stickDeflection(STICK2_X);

  boolean 
  button1 = digitalRead(BUTTON1),
  button2 = digitalRead(BUTTON2),
  stick_button1 = !digitalRead(STICK1_BUTTON),
  stick_button2 = !digitalRead(STICK2_BUTTON);
  
  //logging(xAxis1, yAxis1, xAxis2, yAxis2, button1, button2, stick_button1, stick_button2);

  bytes[0] = xAxis1;
  bytes[1] = yAxis1;
  bytes[2] = xAxis2;
  bytes[3] = yAxis2;
  bytes[4] = button1 + button2 * 2 + stick_button1 * 4 + stick_button2 * 8;
  // simple hash
  bytes[5] = (bytes[0] + 7 * bytes[1] + 11 * bytes[2] + 13 * bytes[3] + 17 * bytes[4]) % 255;
  Bluetooth.write(bytes, 6);
  delay(DELAY);
}

int8_t stickDeflection(uint8_t port) {
  return int8_t((analogRead(port) - 512) / 33);  
}

void logging(int8_t x1, int8_t y1, int8_t x2, int8_t y2, boolean btn1, boolean btn2, boolean s_btn1, boolean s_btn2) {
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
