#include <microLED.h>
#include <FastLED.h> // ФЛ для функции Noise

struct Step {
  int8_t led_amount;
  uint16_t night_mode_bitmask;
};

#define STRIP_LED_AMOUNT 300  // количество чипов WS2811/WS2812 на всех ступеньках. Для WS2811 кол-во чипов = кол-во светодиодов / 3
#define STEP_AMOUNT 15        // количество ступенек

// описание всех ступенек с возможностью подсветки ЛЮБЫХ ступенек в ночном режиме
Step steps[STEP_AMOUNT] = {
  { 20, 0b111111111111111111111 },   // первая ступенька 16 чипов, 0b0100100100100100 - каждый третий чип активен в ночном режиме
  { 20, 0b000000000000000000000 },   // вторая ступенька 16 чипов, 0b0000000000000000 - не активен в ночном режиме
  { 20, 0b000000000000000000000 },   // 3
  { 20, 0b000000000000000000000 },   // 4
  { 20, 0b000000000000000000000 },   // 5
  { 20, 0b000000000000000000000 },   // 6
  { 20, 0b000000000000000000000 },   // 7
  { 20, 0b000000000000000000000 },   // 8
  { 20, 0b000000000000000000000 },   // 9
  { 20, 0b000000000000000000000 },   // 10
  { 20, 0b000000000000000000000 },   // 11
  { 20, 0b000000000000000000000 },   // 12
  { 20, 0b000000000000000000000 },   // 13
  { 20, 0b000000000000000000000 },   // 14
  { 20, 0b111111111111111111111 }    // 15
};
#define AUTO_BRIGHT 1     // автояркость вкл(1)/выкл(0) (с фоторезистором)
#define CUSTOM_BRIGHT 100  // ручная яркость

#define FADR_SPEED 370         // скорость переключения с одной ступеньки на другую, меньше - быстрее
#define TIMEOUT 8            // секунд, таймаут выключения ступенек после срабатывания одного из датчиков движения

#define NIGHT_LIGHT_COLOR mCOLOR(WHITE)  // по умолчанию белый
#define NIGHT_LIGHT_BRIGHT 75  // 0 - 255 яркость ночной подсветки
#define NIGHT_PHOTO_MAX 500   // максимальное значение фоторезистора для отключения подсветки, при освещении выше этого подсветка полностью отключается
#define SMOOTH_DELAY 15 //ПЛАВНОСТЬ ВКЛЮЧЕНИЕ ЭФФЕКТОВ
// пины
// если перепутаны сенсоры - можно поменять их местами в коде! Вот тут
#define SENSOR_START 3   // пин датчика движения
#define SENSOR_END 2     // пин датчика движения
#define STRIP_PIN 13     // пин ленты ступенек
#define PHOTO_PIN A0     // пин фоторезистора

#define ORDER_RGB    // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG ТУТ МОЖЕШЬ ПОПРОБОВАТЬ РАЗНЫЕ ВАРИАНТЫ ЧТОБЫ ЦВЕТА ОТОБРАЖАЛИСЬ ПО ЧЕЛОВЕЧЕСК
#define COLOR_DEBTH 3   // цветовая глубина: 1, 2, 3 (в байтах)

LEDdata stripLEDs[STRIP_LED_AMOUNT];  // буфер ленты ступенек
microLED strip(stripLEDs, STRIP_LED_AMOUNT, STRIP_PIN);  // объект лента (НЕ МАТРИЦА) из-за разного количества диодов на ступеньку!
LEDdata color = mRGB(235, 255, 255); //ВОТ ТУТ МЕНЯТЬ ЦВЕТ РГБ
int8_t effectDirection = 1;
byte curBright = CUSTOM_BRIGHT;
uint32_t timeoutCounter = millis();
uint32_t tmr = millis();
bool systemIdleState = false;
bool systemOffState = false;
bool isNightLight = false;
int steps_start[STEP_AMOUNT];
int8_t minStepLength = steps[0].led_amount;


void setup() {
  Serial.begin(9600);
  setBrightness(curBright);    // яркость (0-255)
  clear();
  show();
  steps_start[0] = 0;
  for (int i = 1; i < STEP_AMOUNT; i++) {
    if (steps[i].led_amount < minStepLength ) {
      minStepLength = steps[i].led_amount;
    }
    steps_start[i] = steps_start[i - 1] + steps[i - 1].led_amount; // вычисляем стартовые позиции каждой ступеньки
  }
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
