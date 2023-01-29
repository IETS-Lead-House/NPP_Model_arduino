#include <SoftwareSerial.h>

#define BLUETOOTH_IN 10
#define BLUETOOTH_OUT 11


SoftwareSerial Bluetooth(BLUETOOTH_IN, BLUETOOTH_OUT);

void setup() {
  pinMode(BLUETOOTH_IN, INPUT);
  pinMode(BLUETOOTH_OUT, OUTPUT);
  
  Serial.begin(9600);
  Bluetooth.begin(38400);

  Serial.println("Controller start");
  //Bluetooth.println("Bluetooth start");
}

void loop() {
    
  if(Serial.available()) { 
    Bluetooth.write(Serial.read());
  }
  
  if(Bluetooth.available()) {
    Serial.write(Bluetooth.read());
  }
  
}
