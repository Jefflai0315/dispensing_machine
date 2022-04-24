#include <FastLED.h> 
#include <elapsedMillis.h>

#define NUM_LEDS 60

elapsedMillis tic;
CRGB leds[NUM_LEDS];
const int ledPin = 6;

///////////////////////////////////////////////// LIGHT v1 ///////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812, ledPin,GBR>(leds, NUM_LEDS); 
  FastLED.setBrightness(50);
  for (int i=0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;}
  
}
int s ;

void loop()
{
  if (Serial.available()>0){
    s= Serial.readStringUntil(',').toInt();
    Serial.println(s);
    if (s == 2){
    for (int i ; NUM_LEDS; i++){
    leds[i] = CRGB(127, 255, 64);
    FastLED.show();
    delay(50);
    if (i == NUM_LEDS){
      delay(5000);
      break ;
    }
  }
  delay(5000);
  fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
    }
  }
  
  }
 
/////////////////////////////////////////////////////// LIGHT v2 ///////////////////////////////////////////////////////////////
// void setup() {
//   Serial.begin(115200);
//   Serial.println("FastLed Test");

//   FastLED.addLeds<WS2812, ledPin, GBR>(leds, NUM_LEDS); 
//   FastLED.setBrightness(50);
//   for (int i=0; i < NUM_LEDS; i++) {
//     leds[i] = CRGB::Black;
//   }
// }

// void loop() {
//   if (tic >= 333) {
//     /* Shift prior colors down all the LEDs */
//     for (int i=NUM_LEDS-1; i > 0; i--) {
//       leds[i] = leds[i - 1];
//     }
//     /* Turn the first LED on with a random color */
//     uint8_t red = random8();
//     uint8_t green = random8();
//     uint8_t blue = random8();
//     leds[0].setRGB(red, green, blue);

//     FastLED.show();
//     tic = 0;
//   }
// }