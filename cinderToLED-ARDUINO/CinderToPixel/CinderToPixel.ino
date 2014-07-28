#include <Adafruit_NeoPixel.h>

#define PIN 6
#define PIN2 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(60, PIN2, NEO_GRB + NEO_KHZ800);

String incomingString;

uint8_t yPix;
uint8_t xPix;

void setup() {
  // initialize serial
  Serial.begin(9600);
  
  // serial handshake
  establishContact();
  
  // set briteness and initialize all pixel strips
  strip.setBrightness(40);
  strip2.setBrightness(40);
  strip.begin();
  strip2.begin();
  strip.show(); 
  strip2.show();
}

void loop() {
   while (Serial.available() > 0) {
     //serial recieved in this format "0.923769,0.519444"
     //put incomingString into an char array for both x and y
      incomingString = Serial.readStringUntil(',');
      char xVal[incomingString.length()+1];
      incomingString.toCharArray(xVal, sizeof(xVal)); 
      incomingString = Serial.readStringUntil('\n');
      char yVal[incomingString.length()+1];
      incomingString.toCharArray(yVal, sizeof(yVal)); 
      
      float x = atof(xVal);
      float y = atof(yVal);

      //scale x to pixelStrip length
      xPix = 60 - (int) (x*60);
      //throw out any garbage numbers
      if(xPix<60 && xPix>0){
          Serial.println(xPix);
          ballFollow( strip.Color(255,127,0), xPix);
      }
   }
}

void establishContact() {                                //serial handshake to insure cinder connects to the arduino
    while (Serial.available() <= 0) {
        Serial.print('*');   // send a capital A
        delay(300);  
      }
}


//this function created some nice light trails as the the ball moved up and down the x axis

void ballFollow( uint32_t c, uint8_t pixNum){
   for (int i = 0; i < pixNum; i++){
        strip.setPixelColor(pixNum, c);
        strip.setPixelColor(pixNum+1, c*.9);
        strip.setPixelColor(pixNum+2, c*.83);
        strip.setPixelColor(pixNum-1, c*9);
        strip.setPixelColor(pixNum-2, c*83);
        strip.setPixelColor(i, strip.getPixelColor(i)*.75);
        strip2.setPixelColor(pixNum, c);
        strip2.setPixelColor(pixNum+1, c*.9);
        strip2.setPixelColor(pixNum+2, c*.83);
        strip2.setPixelColor(pixNum-1, c*9);
        strip2.setPixelColor(pixNum-2, c*83);
        strip2.setPixelColor(i, strip.getPixelColor(i)*.75);
   }
   for (int i = strip.numPixels(); i > pixNum; i--){
       strip.setPixelColor(i, strip.getPixelColor(i)*.75);
       strip2.setPixelColor(i, strip.getPixelColor(i)*.75);
   }
   strip.show();
   strip2.show();
}




