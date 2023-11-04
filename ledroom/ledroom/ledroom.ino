#include <microLED.h>
#include <FastLED.h> // ФЛ для функции Noise

#define LED_AMOUNT 300
#define BRIGHTNESS 100
#define STRIP_PIN 13

#define ORDER_RGB    // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG ТУТ МОЖЕШЬ ПОПРОБОВАТЬ РАЗНЫЕ ВАРИАНТЫ ЧТОБЫ ЦВЕТА ОТОБРАЖАЛИСЬ ПО ЧЕЛОВЕЧЕСК
#define COLOR_DEBTH 3   // цветовая глубина: 1, 2, 3 (в байтах)

LEDdata stripLEDs[LED_AMOUNT];  // буфер ленты ступенек
microLED strip(stripLEDs, LED_AMOUNT, STRIP_PIN);  // объект лента (НЕ МАТРИЦА) из-за разного количества диодов на ступеньку!

byte curBright = CUSTOM_BRIGHT;

void setup() {
  Serial.begin(9600);
  setBrightness(curBright);    // яркость (0-255)
  clear();
  show();
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void setBrightness(int brightness) {
  strip.setBrightness(brightness);
}

void show() {
  strip.show();
}

void clear() {
  strip.clear();
}
