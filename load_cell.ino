#include <Wire.h>
#include <HX711.h> 
#include <FastLED.h> 
#include <elapsedMillis.h>

#define NUM_LEDS 60

elapsedMillis tic;
HX711 loadcell;
CRGB leds[NUM_LEDS];
const int ledPin = 6;
const int LOADCELL_DOUT_PIN = 4; // HX711 circuit wiring
const int LOADCELL_SCK_PIN = 5;  // HX711 circuit wiring
float calibration_factor =  794.73; //adjustment setting for loadcell
float w = 0; // weight calculation
int l;

enum state{wait, taring, weigh, done};

void setup()
{    
  loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadcell.set_scale(calibration_factor);
  loadcell.tare();
  Wire.begin(0); 
  Wire.onReceive(receiveEvent); 
  Serial.begin(9600);

  FastLED.addLeds<WS2812, ledPin,GBR>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  for (int i=0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}
state x = 0;

union floatToBytes { 
    char buffer[4];
    float weightReading;
} convertor;


void loop()
{
  if (x == wait)
  {
      Serial.println("waiting");
  }
  else if (x == done){
    x= wait;
    
  }
  else if(x == taring)
  {
    Serial.println("taring"); 
    loadcell.tare();
  }
  else if(x == weigh)
  {
    Serial.println("weighing"); 
    Wire.beginTransmission(1); // Send to Slave #1
    convertor.weightReading = loadcell.get_units(5);
    Serial.println(convertor.weightReading);
    Wire.write(convertor.buffer, 4);             // Send one byte  
    Wire.endTransmission();    // Stop sending
    delay(100); 
  }
  
}
     
void receiveEvent(int howMany)
{
  x = Wire.read();       // receive a byte as character
}