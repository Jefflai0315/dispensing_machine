#include <AccelStepper.h>
#include <Stepper.h>
#include <MultiStepper.h>
#include <Wire.h> 

int M1dirpin = 5;  //Motor X direction pin
int M1steppin = 2; //Motor X step pin
int M1en=9;  //Motor X enable pin

int M2dirpin = 6;  //Motor Y direction pin
int M2steppin = 3; //Motor Y step pin
int M2en=10;  //Motor Y enable pin

int M3dirpin = 7;  //Motor Z direction pin
int M3steppin = 4; //Motor Z step pin
int M3en=11;  //Motor Z enable pin

int M4dirpin = 13;  //Motor A direction pin
int M4steppin = 12; //Motor A step pin

float weight = 0; 

enum state{wait, taring, weighing, done};
byte y = 2;

uint8_t index = 0;

union floatToBytes {
    char buffer[4];
    float weightReading;
} convertor;

const int NUM_MOTORS = 4; 
long button_time =0;
long last_button_time;

AccelStepper stepper1(AccelStepper::DRIVER, M1steppin,M1dirpin);
AccelStepper stepper2(AccelStepper::DRIVER, M2steppin,M2dirpin);
AccelStepper stepper3(AccelStepper::DRIVER, M3steppin,M3dirpin);
AccelStepper stepper4(AccelStepper::DRIVER, M4steppin,M4dirpin);


//////////////////////////////////////////////////////////  COREXY CLASS //////////////////////////////////////////////////////////////
class COREXY 
{
  private : 
    long positions[NUM_MOTORS] = {0, 0, 0, 0};
    int steps_per_mm = 1;  
    MultiStepper steppers;
    AccelStepper allSteppers[NUM_MOTORS]; 
    long stepperCurrentPos[NUM_MOTORS] = {0, 0, 0, 0};
  
  public: 
    COREXY(AccelStepper stepperMotors[4])
    {
      for (int i=0; i<NUM_MOTORS; i++)
      {
        allSteppers[i] = stepperMotors[i];  
        steppers.addStepper(stepperMotors[i]); 
      }

      steppers.moveTo(positions);
    }

  void moveMotors(int target_x=0, int target_y=0, int target_z=0, int end_effector_pos=0)
  { 

    long yMotorVal[2] = {0,0};
    long xMotorVal[2] = {0,0};
    
    long x_off = target_x - positions[0]; 
    long y_off = target_y - positions[1];
    long z_off = target_z - positions[2];
    long end_effector_off = end_effector_pos - positions[3]; 


    long* y_steps = move_y(y_off,yMotorVal); 
    long* x_steps = move_x(x_off,xMotorVal); 

    positions[0] = target_x;
    positions[1] = target_y; 
    positions[2] = target_z; 
    positions[3] = 0;

    stepperCurrentPos[0] = stepperCurrentPos[0] + y_steps[0]+ x_steps[0];
    stepperCurrentPos[1] = stepperCurrentPos[1] + y_steps[1]+ x_steps[1];
    stepperCurrentPos[2] = stepperCurrentPos[2] + z_off;
    stepperCurrentPos[3] = stepperCurrentPos[3] + end_effector_pos;

    steppers.moveTo(stepperCurrentPos);
    steppers.runSpeedToPosition();
  };
  
  long rescale(int dist_to_travel)
  { 
    return dist_to_travel * steps_per_mm;
  };

  long *move_y(int y_off, long int motorVal[2])
  {  

    motorVal[0] =  rescale(-y_off);
    motorVal[1] = rescale(y_off); 
    return motorVal;
  };

  long *move_x(int x_off, long int motorVal[2])
  { 

    motorVal[0] = rescale(x_off); 
    motorVal[1] = rescale(x_off); 
    return motorVal; 
  };

  void moveTo(int targetX, int targetY)
  {
    moveMotors(targetX, targetY, positions[2], positions[3]);
  }

  void moveZ(int target_z){ 
    moveMotors(positions[0], positions[1], target_z, positions[3]);
  }

  void moveEndEffector(int end_effector_pos)
  {
    moveMotors(positions[0], positions[1], positions[2], end_effector_pos);
  }

  void toggleZ(bool isExtend)
  {
    if (isExtend)
    {
      moveZ(425);
    }
    else
    {
      moveZ(0);
    }
  }
 

  void home() //homing function
  {
    bool reachX = false; 
    bool reachY = false; 
    int x = -5;
    int y = -5;
    while (!reachX | !reachY){
      if (digitalRead(M1en) ==HIGH){
        reachX = true;
        x = 0;
      }
      if (digitalRead(M2en) ==HIGH){
        reachY = true;
        y = 0;
      }
        moveTo(positions[0] + x, positions[1] + y);
     }
    reachY = false; 
    reachX = false;
    positions[0] = 0; 
    stepperCurrentPos[0] = 0;
    positions[1] = 0; 
    stepperCurrentPos[1] = 0;  
    positions[2] = 0; 
    stepperCurrentPos[2] = 0; 
    positions[3] = 0; 
    stepperCurrentPos[3] = 0;  
    steppers.reset();
  };


  void dispense(int x, int y, float w) // a sequence of instruction to dispense
  {
    Serial.println(x);
    Serial.println(y);
    moveTo(x+100, y);
    // move z axis forward 
    toggleZ(true);
    moveTo(x,y); 
    // send through I2C to the other arduino to tare and weigh 
    sendMessage(taring,100); 
    sendMessage(weighing); 
    while(weight < w)
    {
      moveEndEffector(1);
    }
    weight = 0;
    sendMessage(wait);  
    //retract z axis  
    moveTo(x+100, y);
    toggleZ(false);
  }

  void sendMessage(state weighingState, int delayTime=0) // function for I2C communication
  { 
    Serial.println(weighingState);
    Wire.beginTransmission(0); 
    Wire.write(weighingState);             
    Wire.endTransmission();
    delay(delayTime); 
  }

};
//////////////////////////////////////////////////////// END OF COREXY CLASS//////////////////////////////////////////////////////////////////


void receiveEvent(int howMany) // function for I2C communication
{
  while (Wire.available()){
    convertor.buffer[index] = Wire.read();
    index++;
  }  
  index = 0;
  weight = convertor.weightReading; 
  Serial.println(convertor.weightReading);      
}



//////////////////////////////////////////////////////// START OF CODE //////////////////////////////////////////////////////////////
AccelStepper accelsteppers[NUM_MOTORS];
COREXY corexy = COREXY(accelsteppers);  
int l = 0; // x coordinate 
int m = 0;  // y coordinate
float n  =0; // weight


void setup() {
    Serial.begin(9600);
    accelsteppers[0] = stepper1; 
    accelsteppers[1] = stepper2; 
    accelsteppers[2] = stepper3;
    accelsteppers[3] = stepper4;

    for (int i=0; i<NUM_MOTORS; i++) 
    {
      accelsteppers[i].setMaxSpeed(200);
      accelsteppers[i].setCurrentPosition(0);
    }
    
    corexy = COREXY(accelsteppers);
    pinMode(M1en, INPUT); 
    pinMode(M2en, INPUT); 

    Wire.begin(1); 
    Wire.onReceive(receiveEvent); 
}


void loop() {

while(Serial.available()>0) {
  l = Serial.readStringUntil(',').toInt();
  m = Serial.readStringUntil(',').toInt();
  n = Serial.readStringUntil(',').toInt();
  if (l == 1){
//    corexy.home();
    corexy.moveTo(0,0);
  }else if (l == 8){
     corexy.toggleZ(true);
     
  }else if (l == 7){
     corexy.toggleZ(false);
      
  }else if ( l == 9){
      corexy.moveTo(550,700); //go to center
      corexy.sendMessage(done);
      Serial.println("done");
   }else {
      Serial.println(l);
      Serial.println(m);
      Serial.println(n);
      corexy.dispense(l,m,n);  
    }
   }
}