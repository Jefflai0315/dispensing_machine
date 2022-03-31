#include <HX711.h>
#include <elapsedMillis.h>
#include <AccelStepper.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//#define motorPin1 8
//#define motorPin2 9
//#define motorPin3 10
//#define motorPin4 11
//#define MOTORWIRES 4
//AccelStepper stepperX(MOTORWIRES, motorPin1, motorPin2, motorPin3, motorPin4);

int M1dirpin = 4;
int M1steppin = 5;
int M2dirpin = 7;
int M2steppin = 6;

elapsedMillis sinceTest;
 
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

// 2. Adjustment settings
//const long LOADCELL_OFFSET = 50682624;
//const long LOADCELL_DIVIDER = 5895655;
float calibration_factor = -983.81; //-793.49;
float w = 0;

HX711 loadcell;

 
void setup() {
  lcd.begin();
  lcd.print(" HELLO ");
  lcd.setCursor(0,1);
  lcd.print(" POWDERFUL ");
  delay(1000);
  lcd.clear();
  Serial.begin(9600);
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadcell.set_scale(calibration_factor);
  loadcell.tare();
//loadcell.set_scale(LOADCELL_DIVIDER);
//loadcell.set_offset(LOADCELL_OFFSET);
//  stepperX.setMaxSpeed(200.0);
//  stepperX.setAcceleration(20.0);
//  stepperX.setSpeed(200);
//  stepperX.moveTo(2048*180);
  pinMode(M1dirpin,OUTPUT);
  pinMode(M1steppin,OUTPUT);
  pinMode(M2dirpin,OUTPUT);
  pinMode(M2steppin,OUTPUT);

}
 
void loop() {

  delayMicroseconds(2);
  digitalWrite(M1dirpin,LOW);
  digitalWrite(M2dirpin,LOW);
Serial.println(sinceTest);
//  stepperX.run();
//      w = loadcell.get_units(15) ;
//  lcd.setCursor(0,0);
//    lcd.print("Measured Weight");
//     lcd.setCursor(0,1);
//     lcd.print(w+0.3, 1);
//     lcd.print("g ");
//  else if (w< 100*0.98){
//    stepperX.setCurrentPosition(0);
//    stepperX.moveTo(1000);
//    stepperX.run();
//  }else {stepperX.setCurrentPosition(0);
//    stepperX.moveTo(0);
//    stepperX.run();}
//
//
//     }
//Serial.println(sinceTest);
// stepperX.run();
// Serial.println(sinceTest);

while ( w <100){
    digitalWrite(M1steppin,LOW);
    digitalWrite(M2steppin,LOW);
    Serial.println(w);
    delayMicroseconds(2);
    digitalWrite(M1steppin,HIGH);
    digitalWrite(M2steppin,HIGH);
    delay(1);
if (sinceTest>=200)
{
   Serial.println(sinceTest);
    lcd.setCursor(0,0);
   
    lcd.print("Measured Weight");
     lcd.setCursor(0,1);
w=loadcell.get_units(1);
//if (w>90){stepperX.setSpeed(1);}
//if (w>75){stepperX.setSpeed(8);}
     if (w <0) {
     lcd.print(0.0, 1);
     lcd.print("g ");}
     else {lcd.print(w+0.3, 1);
     lcd.print("g ");}
//    
     Serial.println(sinceTest);
     sinceTest= sinceTest -200;
 }
}
    
    if (w>100){
//    stepperX.stop();
    Serial.println(w); 
    Serial.println("Wait...!"); 
    delay(2000);
    w =loadcell.get_units(1); 
    
  }
//  if (stepperX.distanceToGo() == 0){
//    stepperX.setCurrentPosition(0);
//    stepperX.moveTo(2048*2);
//  }
}


  