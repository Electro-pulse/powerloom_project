#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int PD_PIN =2; // D2 used for power-down detection (INT.0)
int WC_PIN=3; //  D3 used for counting wefts input
int inti_pin=13;

const int PIN_4=4, PIN_5=5, PIN_6=6;

int state1=0, state2=0, state3=0;
int metre_a=0, metre_b=0, metre_c=0, last_count=0, current_metre=0, user=0, target=0;
int flag=0;
int wefts=0, max_count=0;
int inti_metre=5;
int timer=0;

void powercut_ISR(){           // Powerdown interrupt
EEPROM.put(0, metre_a);
EEPROM.put(2, metre_b);
EEPROM.put(4, metre_c);
EEPROM.put(6, wefts);
EEPROM.put(8, max_count);
EEPROM.put(10, current_metre);
}

void weft_counting_ISR(){       //Metre reset interrupt
 wefts+=1;
}

void get_user(){
  // Getting current user
  if(digitalRead(PIN_4)==HIGH){
    user=1;
    lcd.clear();
    lcd.print("Current user: A");
    delay(500);
    lcd.clear();
  }
  else if(digitalRead(PIN_5)==HIGH){
    user=2;
    lcd.clear();
    lcd.print("Current user: B");
    delay(500);
    lcd.clear();
  }
  else if(digitalRead(PIN_6)==HIGH){
    user=3;
    lcd.clear();
    lcd.print("Current user: C");
    delay(500);
    lcd.clear();
  }
  else{
    user=0;
    lcd.clear();
    lcd.print("Select the user");
    delay(500);
    lcd.clear();
    get_user();
  }
}

void setup()
{
EEPROM.get(0, metre_a);
EEPROM.get(2, metre_b);
EEPROM.get(4, metre_c);
EEPROM.get(6, last_count);
EEPROM.get(8, max_count);
EEPROM.get(10, current_metre);
attachInterrupt(digitalPinToInterrupt(PD_PIN), powercut_ISR,FALLING); // Set-up Interrupt Service Routine (ISR)  
attachInterrupt(digitalPinToInterrupt(WC_PIN),weft_counting_ISR,RISING);  
pinMode(PIN_4,INPUT);
pinMode(PIN_5,INPUT);
pinMode(PIN_6,INPUT);
pinMode(inti_pin,OUTPUT);
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Initializing...");
delay(1000);
lcd.clear();
get_user();
lcd.clear();
lcd.print("Set count please");
delay(3000);
lcd.clear();
}

void loop() {
lcd.clear();
int new_state1=digitalRead(PIN_4);
int new_state2=digitalRead(PIN_5);
int new_state3=digitalRead(PIN_6);
if( (new_state1!=state1) | (new_state2!=state2) | (new_state3!=state3) ){
  get_user();
  state1=new_state1;
  state2=new_state2;
  state3=new_state3;
}
if( (digitalRead(9)) || (digitalRead(10)) )  
{

while(digitalRead(9))     // upcount
{
lcd.clear();
lcd.print("   COUNT ");
max_count++;
if(max_count == 3000){
  max_count=2000;
}
lcd.print(max_count);
delay(100);
}
while(digitalRead(10))  // downcount
{
max_count--;
if(max_count == 2000){
  max_count=3000;
}
lcd.clear();
lcd.print("   COUNT ");
lcd.print(max_count);
delay(100);
}
delay(300);
lcd.clear();
flag=1;
}
if(flag){
  target=max_count;
}
else{
  target=max_count;
  wefts=last_count;
}
if(wefts>target+1){
  wefts=0;
}
// Meter calculation
if(wefts==target){
    current_metre+=1;
    if(user==1){
      metre_a+=1;
    }
    else if(user==2){
      metre_b+=1;
    }
    else if(user==3){
      metre_c+=1;
    }
    wefts=0;
  }
lcd.print("Weft:");
lcd.print(wefts);
lcd.setCursor(10,0);
lcd.print("Mtr:");
lcd.print(current_metre);
lcd.setCursor(0,1);
lcd.print("A:");
lcd.print(metre_a);
lcd.print(" B:");
lcd.print(metre_b);
lcd.print(" C:");
lcd.print(metre_c);
delay(200);

// Intimating the user
if(current_metre==inti_metre){
  digitalWrite(inti_pin,HIGH);
  current_metre=0;
}

// Resetting all metre values to zero
if(digitalRead(11)==HIGH){
  wefts=0;
  current_metre=0;
}
if(digitalRead(inti_pin)==HIGH){
  timer+=1;
  if(timer==25){
    digitalWrite(inti_pin,LOW);
    timer=0;
  }
}
if(digitalRead(12)==HIGH){
  metre_a=0;
  metre_b=0;
  metre_c=0;
  last_count=0;
  current_metre=0;
  user=0;
  target=0;
  wefts=0;
  max_count=50;
}
}
