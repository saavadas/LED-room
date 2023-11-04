#include <FastLED.h> // ФЛ для функции Noise

#define BRIGHTNESS 100
#define DATA_PIN 13
#define NUM_LEDS 300

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void setBrightness(int brightness) {
  FastLED.setBrightness(brightness);
}

void show() {
  FastLED.show();
}

void clear() {
  FastLED.clear();
}

/*
  random16()      == random from 0..65535
  Serial.readBytes( (char*)(&leds[i]), 3); // read three bytes: r, g, and b.
  leds[i].nscale8( 192);
  leds[i].fadeToBlackBy( 64 );



*/
