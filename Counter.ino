#include <EEPROM.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27
#define bLED 12
#define yLED 13
#define resetb 3
#define counterb 2
#define BACKLIGHT_PIN 3

int currentval;
int currentptr= EEPROM.read(0);
int totalval;

float LDR;
int but;
int state = LOW;
bool active = false;
int prevstate = HIGH;
LiquidCrystal_I2C lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  currentptr = EEPROM.read(0);
  Serial.println("Start at "+(String)currentptr);
  // ---------------------------------------- LCD SET UP BEGIN
  lcd.begin (16, 2);
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();

  lcd.setCursor(0, 0);
  lcd.print("Total Count");

  //lcd.setCursor(4, 1);
  //lcd.print("test "+(String)getTotal());
  lcd.print("                ");
  printTotal();
  // ---------------------------------------- LCD SET UP END

  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  digitalWrite(yLED, HIGH);
  //attachInterrupt(digitalPinToInterrupt(2),check,FALLING);
  //attachInterrupt(digitalPinToInterrupt(3),clearEEPROM,FALLING);
  if(currentptr==0){
    currentptr = 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  LDR = analogRead(A0);
  
  //Serial.println("LDR value : "+ (String)LDR);
  if(LDR < 190){
    active = true;
    digitalWrite(bLED, HIGH);
    state = HIGH;
  }
  else{
    active = false;
    digitalWrite(bLED, LOW);
    state = LOW;
  }
  if(active){
    but = digitalRead(2);
    if(but==0 && prevstate == HIGH){
      delay(300);
      updateCount();
      prevstate = LOW;
    }
    else if (but ==1){
      prevstate = HIGH;
    }
  }
  int but2 = digitalRead(3);
  if(but2 == 0){
    delay(300);
    clearEEPROM();
  }
}
void check(){
  Serial.println("Interupt");
//  if(active){
//    but = digitalRead(2);
//    if(but==0){
//      updateCount();
//      if(state == HIGH){
//      state = LOW;
//      }
//      else{
//        state = HIGH;
//      }
//    } 
//  }
}

void clearEEPROM(){
  digitalWrite(12,LOW);
  digitalWrite(13,LOW);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.write(0, 1);
  //EEPROM.write(1, 254);
  currentptr = 1;
  Serial.println("CLEARED");
  digitalWrite(12,HIGH);
  digitalWrite(13,state);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  printTotal();
}
int getTotal(){
  int mult;
  Serial.println("In get total "+(String)currentptr);
  if(currentptr == 1){
    mult = 0;
  }
  else{
    mult = currentptr-1;
  }
  totalval= 255*(mult) + EEPROM.read(currentptr);
  //Serial.println("ptr: "+(String)currentptr+" totalval: "+totalval);
  return totalval;
}
void updateCount(){
  digitalWrite(12,LOW);
  delay(100);
  currentval = EEPROM.read(currentptr);
  if(currentval<255){
    EEPROM.write(currentptr,currentval+1);
  }
  else{
    currentptr +=1;
    EEPROM.write(0,currentptr);
    EEPROM.write(currentptr,1);
  }
  Serial.println("At "+(String)currentptr+" has "+(String)EEPROM.read(currentptr));
  digitalWrite(12,HIGH);
  printTotal();
}
void printTotal(){
  String temp=(String)getTotal();
  Serial.println("In printTotal "+(String)getTotal());
  lcd.setCursor(16-temp.length(), 1);
  lcd.print(temp);
}
