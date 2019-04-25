#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x26
#define I2C_ADDR2 0x27
#define bLED 12
#define yLED 13
#define buzzer A1
#define resetb 2
#define counterb 3
#define BACKLIGHT_PIN 3
// -------------------- Calculator var BEGIN -----------------------
LiquidCrystal_I2C lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);
const byte numRows = 4;
const byte numCols = 4;
unsigned int in1, in2;
float res;
String  input1,input2,func,err,result;
String A, B;
char keymap[numRows][numCols] = {
  {'1', '2', '3', '+'},

  {'4', '5', '6', '-'},

  {'7', '8', '9', '*'},

  {'x', '0', '=', '/'}
};
byte rowPins[numRows] = {11, 10, 9, 8}; //Rows 0 to 3
byte colPins[numCols] = {7, 6, 5, 4}; //Columns 0 to 3
bool inputflag = false;
bool symbolflag = false;
bool resultflag = false;
bool errorflag = false;
//initializes an instance of the Keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
// -------------------- Calculator var END -----------------------


// -------------------- Counter Var BEGIN ------------------------
LiquidCrystal_I2C lcd2(I2C_ADDR2, 2, 1, 0, 4, 5, 6, 7);
int currentval;
int currentptr= EEPROM.read(0);
int totalval;
bool toUpdate = false;
float LDR;
int but;
int state = LOW;
bool active = false;
int prevstate = HIGH;

// -------------------- Counter Var END ------------------------

// ------------------- Calculator Func BEGIN -------------------

String toSTR(float res) {
  result = String(res);
  if (result.length() > 15 || res == 0xFFFF) {
    errorflag = true;
    err = "OVERFLOW";
  }
  return result;
}

String plus(String input1, String input2) {
  in1 = input1.toInt();
  in2 = input2.toInt();
  res = in1 + in2;
  res = (int)res;
  result = toSTR(res);
  return (result);
}

String minus(String input1, String input2) {
  in1 = input1.toInt();
  in2 = input2.toInt();
  res = in1 - in2;
  result = toSTR(res);
  return (result);
}

String mult(String input1, String input2) {
  in1 = input1.toInt();
  in2 = input2.toInt();
  res = in1 * in2;
  result = toSTR(res);
  return (result);
}

String divi(String input1, String input2) {
  in1 = input1.toInt();
  in2 = input2.toInt();
  res = (float)in1 / (float)in2;
  result = toSTR(res);
  return (result);
}

void reset() {
  symbolflag = false;
  inputflag = false;
  resultflag = false;
  errorflag = false;
  input1 = "";
  input2 = "";
  func = "";
  result = "";
}

void getSymbol(char key) {
  if (key == '+') {
    func = "+";
  }
  else if (key == '-') {
    func = "-";
  }
  else if (key == '*') {
    func = "*";
  }
  else if (key == '/') {
    func = "/";
  }
}

void calculate(String fun) {
  if (fun == "+") {
    result = plus(input1, input2);
  }
  else if (fun == "-") {
    result = minus(input1, input2);
  }
  else if (fun == "*") {
    result = mult(input1, input2);
  }
  else if (fun == "/") {
    result = divi(input1, input2);
  }
  resultflag = true;
  symbolflag = false;
}

// ------------------- Calculator Func BEGIN -------------------

// ------------------- Counter Func BEGIN -------------------

void clearEEPROM(){
  digitalWrite(yLED,LOW);
  digitalWrite(bLED,LOW);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.write(0, 1);
  //EEPROM.write(1, 254);
  currentptr = 1;
  Serial.println("CLEARED");
  digitalWrite(yLED,HIGH);
  digitalWrite(bLED,state);
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
  digitalWrite(yLED,LOW);
  digitalWrite(bLED,LOW);
  digitalWrite(buzzer,HIGH);
  Serial.println("I'm in updateCount");
  //delay(100);
  currentval = EEPROM.read(currentptr);
  if(currentval<255){
    Serial.println("Case1");
    EEPROM.write(currentptr,currentval+1);
  }
  else{
    currentptr +=1;
    Serial.println("Case2");
    EEPROM.write(0,currentptr);
    EEPROM.write(currentptr,1);
  }
  Serial.println("Case3");
  Serial.println("At "+(String)currentptr+" has "+(String)EEPROM.read(currentptr));
  Serial.println("Calling Print Total");
  delay(500);
  digitalWrite(buzzer,LOW);
  digitalWrite(yLED,HIGH);
  digitalWrite(bLED,HIGH);
  printTotal();
  
  toUpdate = false;
}
void printTotal(){
  Serial.println("I'm in Print Total");
  String temp=(String)getTotal();
  Serial.println("In printTotal "+(String)getTotal());
  lcd.setCursor(16-temp.length(), 1);
  lcd.print(temp);
}

// ------------------- Counter Func END -------------------

void Interupt (){
  if(active && (digitalRead(3)==0)){
    toUpdate = true;
  }
  else{
    toUpdate = false;
  }
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3), Interupt, CHANGE);
  // --------------- CALCULATOR SET UP BEGIN -------------
  lcd2.begin (16, 2);
  // Switch on the backlight
  lcd2.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd2.setBacklight(HIGH);
  lcd2.home ();

  lcd2.setCursor(1, 0);
  lcd2.print("Korkortor's");

  lcd2.setCursor(4, 1);
  lcd2.print("Calculator");
  // --------------- CALCULATOR SET UP END -------------

  // --------------- COUNTER SET UP BEGIN -------------
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
  lcd.print("                ");
  printTotal();
  pinMode(buzzer,OUTPUT);
  pinMode(bLED,OUTPUT);
  pinMode(yLED,OUTPUT);
  pinMode(counterb,INPUT_PULLUP);
  pinMode(resetb,INPUT_PULLUP);
  digitalWrite(yLED, HIGH);
  digitalWrite(bLED, HIGH);
  // --------------- COUNTER SET UP END ---------------
}

void loop() {
  // --------------------  CALCULATOR LOOP BEGIN ---------------------
  char keyPress = myKeypad.getKey();
  if (keyPress != NO_KEY)
  {

    // ---------------------------------------- NUMBER PART BEGIN
    if (keyPress != '+' && keyPress != '-' && keyPress != '*' && keyPress != '/' && keyPress != 'x' && keyPress != '=')  // check that only a number key has been pressed
    {
      if (resultflag == true && symbolflag == false) {
        reset();
      }
      if ( inputflag == false && symbolflag == false)
      {
        input1 += (char)keyPress;
        if(input1.length()>14){
          errorflag = true;
          err = "OVERFLOW";
        }
        //Serial.println(input1);
      }
      else if (inputflag == true && symbolflag == true)
      {
        input2 += (char)keyPress;
        if(input2.length()+input1.length()+1>16){
          errorflag = true;
          err = "OVERFLOW";
        }
      }
    }
    // ---------------------------------------- NUMBER PART END


    // ---------------------------------------- SYMBOL PART BEGIN

    else if (keyPress == '+' || keyPress == '-' || keyPress == '*' || keyPress == '/') {

      // --------------------- Symbol is pressed after calculate BEGIN
      if (resultflag == true) {
        input1 = result;
        inputflag = true;
        getSymbol(keyPress);
        symbolflag = true;
        result = "";
        resultflag = false;
        input2 = "";
      }
      // -------------------- Symbol is pressed after calculate END

      // -------------------- Symbol is pressed after 2nd input BEGIN
      else if (symbolflag == true && input2.length() != 0 && resultflag == false) {
        calculate(func);
        resultflag = false;
        inputflag = true;
        input1 = result;
        input2 = "";
        getSymbol(keyPress);
        symbolflag = true;
      }
      // -------------------- Symbol is pressed after 2nd input END
      
      // -------------------- Symbol is pressed w/o input1 BEGIN
      else if(input1 == ""){
      errorflag = true;
      err = "SYNTAX";
      }
      // -------------------- Symbol is pressed w/o input1 END

      
      //--------------------- Normal Case BEGIN
      else {
        symbolflag = true;
        inputflag = true;
        getSymbol(keyPress);
      }
      //--------------------- Normal Case END
    }
    // ---------------------------------------- SYMBOL PART END


    // ---------------------------------------- NORMAL CALCULATE PART BEGIN
    else if (keyPress == '=') {
      if (symbolflag == true && inputflag == true && input2 != "") {
        calculate(func);

      }
      else {
        errorflag = true;
        err = "SYNTAX";
      }
    }
    // ---------------------------------------- NORMAL CALCULATE PART END


    // ---------------------------------------- CLEAR PART BEGIN
    else if (keyPress == 'x') {
      reset();
    }
    // ---------------------------------------- CLEAR PART END

    
    // ---------------------------------------- LCD PART BEGIN

    if(errorflag){
      lcd2.clear();
      lcd2.setCursor((16-err.length())/2, 0);
      lcd2.print(err);
      String err2 = "ERROR";
      lcd2.setCursor((16-err2.length())/2, 1);
      lcd2.print(err2);
      reset();
    }

    // -------------------------- No Error BEGIN
    else{
      // ----------------- First Line BEGIN
      A = input1 + func + input2;
      lcd2.clear();
      //Serial.println("A: "+ A);
      lcd2.setCursor(0, 0);
      lcd2.print(A);
      // ----------------- First Line END
  
      // ----------------- Second Line BEGIN
      B = "= " + result;
      //Serial.println("B: "+ B);
      lcd2.setCursor(0, 1);
      lcd2.print("=");
      lcd2.setCursor(16 - result.length(), 1);
      lcd2.print(result);
      // ----------------- Second Line END
    }
    // -------------------------- No Error END
    // ---------------------------------------- LCD PART END
  }
// --------------------  CALCULATOR LOOP END -----------------


 // --------------------  COUNTER LOOP BEGIN -----------------
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
  /*if(active){
    but = digitalRead(counterb);
    if(but==0 && prevstate == HIGH){
      delay(300);
      updateCount();
      prevstate = LOW;
    }
    else if (but ==1){
      prevstate = HIGH;
    }
  }*/
  if(toUpdate){
    updateCount();
  }
  int but2 = digitalRead(resetb);
  if(but2 == 0){
    delay(300);
    clearEEPROM();
  }

 // --------------------  COUNTER LOOP END ----------------- 

}
