#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <TftSpfd5408.h>
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
#define LGRAY   0xC618
#define DGRAY   0x7BEF
#define DRED    0xE800

TftSpfd5408 tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

SoftwareSerial Bluetooth(BLUETOOTH_IN, BLUETOOTH_OUT);

byte bytes[8];
boolean flag = false;
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
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);
  if(!digitalRead(BUTTON2)) {
    //graphics
    tft.fillScreen(LGRAY);
    drawIETS(58,98);
    tft.setTextSize(4);
    drawText(113,20,"IETS",WHITE);
    
    drawButton(272,60);
    drawText(262,30,"R",WHITE);
    
    drawStick(272,200);
    drawText(262,200,"Z",WHITE);
    
    drawButton(48,60);
    drawText(38,30,"L",WHITE);
    
    drawStick(48,200);
    drawText(26,200,"XY",WHITE);
  
    tft.setTextSize(1);
    drawText(243,80,"off magnet",WHITE);
    drawText(23,80,"on magnet",WHITE);
  
    drawText(0,0,"created by:",WHITE);
    drawText(0,10,"Lenskih Andrew",WHITE);
  } else {
    flag = true;
    tft.fillScreen(BLACK);
    tft.setTextColor(GREEN);
    tft.setCursor(0, 0);
  }
}

void loop() {
  // Система
  sendData();
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
  if(flag) {
    static int delta = 0;
    logging(xAxis1, yAxis1, xAxis2, yAxis2, button1, button2, stick_button1, stick_button2);
    tft.setTextColor(GREEN);
    tft.setCursor(0, delta);
    tft_logging(xAxis1, yAxis1, xAxis2, yAxis2, button1, button2, stick_button1, stick_button2);
    delta+=10;
    if(delta > 240) {
       tft.fillScreen(BLACK);
       delta = 0;
    }
  }

  bytes[0] = 17;
  bytes[1] = 19;
  bytes[2] = xAxis1;
  bytes[3] = yAxis1;
  bytes[4] = xAxis2;
  bytes[5] = yAxis2;
  bytes[6] = button1 + button2 * 2 + stick_button1 * 4 + stick_button2 * 8;
  // simple hash
  bytes[7] = (bytes[2] + 7 * bytes[3] + 11 * bytes[4] + 13 * bytes[5] + 17 * bytes[6]) % 255;
  Bluetooth.write(bytes, 8);
  delay(DELAY);
}

int8_t stickDeflection(uint8_t port) {
  return int8_t((analogRead(port) - 512) / 33);  
}

void tft_logging(int8_t x1, int8_t y1, int8_t x2, int8_t y2, boolean btn1, boolean btn2, boolean s_btn1, boolean s_btn2) {
  tft.print("Lx:"); 
  tft.print(x1); 
  tft.print(" Ly:");  
  tft.print(y1); 
  tft.print(" Lb:"); 
  tft.print(s_btn1);  
  tft.print(" Rx:");  
  tft.print(x2); 
  tft.print(" Ry:");  
  tft.print(y2); 
  tft.print(" Rb:");
  tft.print(s_btn2); 
  tft.print(" L:");     
  tft.print(btn1); 
  tft.print(" R:");     
  tft.print(btn2); 
}

void drawText(uint16_t x, uint16_t y, String text, uint16_t textColor) {
  tft.setCursor(x, y);
  tft.setTextColor(textColor);
  tft.println(text);
}

void drawButton(uint16_t x, uint16_t y) {
    //padding
    fillEllipse(x, y, 28, 24, BLACK);
    //button
    fillEllipse(x, y, 20, 16, DRED);
    tft.fillRect(x-20, y-8, 41, 10, DRED);
    fillEllipse(x, y-8, 20, 16, RED);
}

void drawStick(uint16_t x, uint16_t y) {
    //down
    fillEllipse(x,y, 36,26, DGRAY);
    fillEllipse(x,y, 34,24, BLACK);
    //up
    fillEllipse(x,y - 20, 26,16, DGRAY);
    fillEllipse(x,y - 20, 24,14, BLACK);
}

//я слегка обосрался и пришлось делать так
void drawL(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  tft.drawLine(y1+38,x1-40,y2+38,x2-40, WHITE);
  tft.drawLine(y1+39,x1-40,y2+39,x2-40, WHITE);
  tft.drawLine(y1+40,x1-40,y2+40,x2-40, WHITE);
}

void drawIETS(uint16_t y, uint16_t x) {
  tft.fillRoundRect(x, y, 125, 125, 31, ORANGE);
  //l1
  drawL(x+10,y+8,x+32,y+16);
  drawL(x+4,y+16,x+32,y+16);
  drawL(x+2,y+24,x+32,y+16);
  
  drawL(x+2,y+24,x+32,y+36);
  drawL(x+1,y+36,x+32,y+36);
  drawL(x+1,y+44,x+32,y+36);

  drawL(x+1,y+44,x+32,y+52);
  drawL(x+1,y+52,x+32,y+52);
  drawL(x+1,y+62,x+32,y+52);
  //r1
  drawL(x+1,y+62,x+32,y+72);
  drawL(x+1,y+72,x+32,y+72);
  drawL(x+2,y+80,x+32,y+72);

  drawL(x+1,y+80,x+32,y+88);
  drawL(x+1,y+88,x+32,y+88);
  drawL(x+2,y+100,x+32,y+88);

  drawL(x+2,y+100,x+32,y+108);
  drawL(x+4,y+108,x+32,y+108);
  drawL(x+10,y+116,x+32,y+108);
  //l2
  drawL(x+32,y+16,x+56,y+18);
  drawL(x+32,y+36,x+56,y+36);
  drawL(x+32,y+52,x+56,y+52);
  //r2
  drawL(x+32,y+72,x+56,y+72);
  drawL(x+32,y+88,x+56,y+88);
  drawL(x+32,y+108,x+56,y+106);
  //l3
  drawL(x+56,y+18,x+88,y+32);
  drawL(x+56,y+36,x+88,y+52);
  drawL(x+56,y+52,x+88,y+32);
  drawL(x+56,y+52,x+88,y+72);
  //r3
  drawL(x+56,y+72,x+88,y+52);
  drawL(x+56,y+72,x+88,y+92);
  drawL(x+56,y+88,x+88,y+72);
  drawL(x+56,y+106,x+88,y+92);
  //l4
  drawL(x+88,y+32,x+124,y+32);
  drawL(x+88,y+52,x+124,y+52);
  //r4
  drawL(x+88,y+72,x+124,y+72);
  drawL(x+88,y+92,x+124,y+92);
}

void fillEllipse(int _x, int _y, double w, double h, uint16_t color) {
  int x0 = w;
  int dx = 0;
  
  // do the horizontal diameter
  for (int x = -w; x <= w; x++)
      tft.drawPixel(_x + x, _y, color);
  
  // now do both halves at the same time, away from the diameter
  for (int y = 1; y <= h; y++)
  {
      int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
      for ( ; x1 > 0; x1--)
          if (x1*x1/w/w + y*y/h/h <= 1.0)
              break;
      dx = x0 - x1;  // current approximation of the slope
      x0 = x1;
  
      for (int x = -x0; x <= x0; x++)
      {
          tft.drawPixel(_x + x, _y - y, color);
          tft.drawPixel(_x + x, _y + y, color);
      }
  }
}
