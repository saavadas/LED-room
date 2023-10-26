/*
  Новые функции в прошивке 1.2 описаны в README.MD
  Автор: Геннадий Дегтерёв, 2021
  gennadij@degterjow.de

  Скетч к проекту "Подсветка лестницы"
  Страница проекта (схемы, описания): https://alexgyver.ru/ledstairs/
  Исходники на GitHub: https://github.com/AlexGyver/LEDstairs
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2019
  https://AlexGyver.ru/
  Модифициравно для собственных целей: https://github.com/saavadas
*/

struct Step {
  int8_t led_amount;
  uint16_t night_mode_bitmask;  
};

#define STRIP_LED_AMOUNT 300  // количество чипов WS2811/WS2812 на всех ступеньках. Для WS2811 кол-во чипов = кол-во светодиодов / 3
#define STEP_AMOUNT 15        // количество ступенек

// описание всех ступенек с возможностью подсветки ЛЮБЫХ ступенек в ночном режиме
Step steps[STEP_AMOUNT] = { 
{ 16, 0b010010010010010010010 },   // первая ступенька 20 чипов, 0b010010010010010010010 - каждый третий чип активен в ночном режиме
{ 16, 0b000000000000000000000 },   // вторая ступенька 20 чипов, 0b000000000000000000000 - не активен в ночном режиме
{ 16, 0b000000000000000000000 },   // 3
{ 16, 0b000000000000000000000 },   // 4
{ 16, 0b000000000000000000000 },   // 5
{ 16, 0b000000000000000000000 },   // 6
{ 16, 0b000000000000000000000 },   // 7
{ 16, 0b000000000000000000000 },   // 8
{ 16, 0b000000000000000000000 },   // 9
{ 16, 0b000000000000000000000 },   // 10
{ 16, 0b000000000000000000000 },   // 11
{ 16, 0b000000000000000000000 },   // 12
{ 16, 0b000000000000000000000 },   // 13
{ 16, 0b000000000000000000000 },   // 14
{ 16, 0b010010010010010010010 }    // 15
};
  
#define CUSTOM_BRIGHT 100  // ручная яркость

#define FADR_SPEED 300         // скорость переключения с одной ступеньки на другую, меньше - быстрее
#define START_EFFECT RAINBOW   // режим при старте COLOR, RAINBOW, FIRE
#define ROTATE_EFFECTS 1      // вкл(1)/выкл(0) - автосмена эффектов
#define TIMEOUT 15            // секунд, таймаут выключения ступенек после срабатывания одного из датчиков движения

#define NIGHT_LIGHT_COLOR mCOLOR(WHITE)  // по умолчанию белый
#define NIGHT_LIGHT_BRIGHT 50  // 0 - 255 яркость ночной подсветки
#define NIGHT_PHOTO_MAX 500   // максимальное значение фоторезистора для отключения подсветки, при освещении выше этого подсветка полностью отключается

// пины
// если перепутаны сенсоры - можно поменять их местами в коде! Вот тут
#define SENSOR_START 3   // пин датчика движения
#define SENSOR_END 2     // пин датчика движения
#define STRIP_PIN 13     // пин ленты ступенек

#define ORDER_BGR       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#define COLOR_DEBTH 2   // цветовая глубина: 1, 2, 3 (в байтах)

// для разработчиков
#include <microLED.h>
#include <FastLED.h> // ФЛ для функции Noise

// ==== удобные макросы ====
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define FOR_j(from, to) for(int j = (from); j < (to); j++)
#define FOR_k(from, to) for(int k = (from); k < (to); k++)
#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flag = millis() - tmr >= (x);\
  if (flag) tmr = millis();\
  if (flag)
//===========================

LEDdata stripLEDs[STRIP_LED_AMOUNT];  // буфер ленты ступенек
microLED strip(stripLEDs, STRIP_LED_AMOUNT, STRIP_PIN);  // объект лента (НЕ МАТРИЦА) из-за разного количества диодов на ступеньку!

int effSpeed;
int8_t effectDirection;
byte curBright = CUSTOM_BRIGHT;
enum {COLOR, RAINBOW, FIRE, EFFECTS_AMOUNT} curEffect = START_EFFECT;
byte effectCounter;
uint32_t timeoutCounter;
bool systemIdleState;
bool systemOffState;
int steps_start[STEP_AMOUNT];

struct PirSensor {
  int8_t effectDirection;
  int8_t pin;
  bool lastState;
};

PirSensor startPirSensor = { 1, SENSOR_START, false};
PirSensor endPirSensor = { -1, SENSOR_END, false};

CRGBPalette16 firePalette;

int8_t minStepLength = steps[0].led_amount;


void setup() {
  Serial.begin(9600);
  setBrightness(curBright);    // яркость (0-255)
  clear();
  show();  
  

  firePalette = CRGBPalette16(
                  getFireColor(0 * 16),
                  getFireColor(1 * 16),
                  getFireColor(2 * 16),
                  getFireColor(3 * 16),
                  getFireColor(4 * 16),
                  getFireColor(5 * 16),
                  getFireColor(6 * 16),
                  getFireColor(7 * 16),
                  getFireColor(8 * 16),
                  getFireColor(9 * 16),
                  getFireColor(10 * 16),
                  getFireColor(11 * 16),
                  getFireColor(12 * 16),
                  getFireColor(13 * 16),
                  getFireColor(14 * 16),
                  getFireColor(15 * 16)
                );
  // определяем минимальную ширину ступеньки для корректной работы эффекта огня
  steps_start[0] = 0;
  FOR_i(1, STEP_AMOUNT) {
    if (steps[i].led_amount < minStepLength) {
      minStepLength = steps[i].led_amount;
    }
    steps_start[i] = steps_start[i-1] + steps[i-1].led_amount; // вычисляем стартовые позиции каждой ступеньки
  }
  delay(100);
  clear();
  show();
}

void loop() {
  if (systemIdleState || systemOffState) {
    handleNightLight();
    delay(50);
  } else {
    effectFlow();
    handleTimeout();
  }
}


void handleNightLight() {
  EVERY_MS(60000) {
    nightLight();
  }
}

void nightLight() {
  if (systemOffState) {
    Serial.println("System OFF ");
    clear();
    show();
    return;
  }
  animatedSwitchOff(NIGHT_LIGHT_BRIGHT);
  clear();
  FOR_i(0, STEP_AMOUNT) {
    // циклически сдвигаем маску, чтобы диоды не выгорали
    if (steps[i].night_mode_bitmask) {
      steps[i].night_mode_bitmask = (uint16_t) steps[i].night_mode_bitmask >> 1 | steps[i].night_mode_bitmask << 15;
      fillStepWithBitMask(i, NIGHT_LIGHT_COLOR, steps[i].night_mode_bitmask);
    }
  }
  animatedSwitchOn(NIGHT_LIGHT_BRIGHT);
}

void handleTimeout() {
  if (millis() - timeoutCounter >= (TIMEOUT * 1000L)) {
    systemIdleState = true;
    if (effectDirection == 1) {
      stepFader(0, 1);
    } else {
      stepFader(1, 1);
    }
    nightLight();
  }
}

// крутилка эффектов в режиме активной работы
void effectFlow() {
  static uint32_t tmr;
  if (millis() - tmr >= effSpeed) {
    tmr = millis();
    switch (curEffect) {
      case COLOR: staticColor(effectDirection, 0, STEP_AMOUNT); break;
      case RAINBOW: rainbowStripes(-effectDirection, 0, STEP_AMOUNT); break; // rainbowStripes - приёмный
      case FIRE: fireStairs(effectDirection, 0, 0); break;
    }
    show();
  }
}
