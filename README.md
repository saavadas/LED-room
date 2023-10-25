Чтобы не выжигать одни и теже диоды, маска инвертируется каждые 60 секунд  
Цвет ночной подсветки можно определить через функции библиотеки microLED:
* mCOLOR(WHITE) или SILVER,GRAY,BLACK,RED,MAROON,ORANGE,YELLOW,OLIVE,LIME,GREEN,AQUA,TEAL,BLUE,NAVY,MAGENTA,PURPLE
* mRGB(byte r, byte g, byte b);   // RGB 255, 255, 255
* mWHEEL(int color);              // цвета 0-1530
* mHEX(uint32_t color);           // HEX цвет
* mHSV(byte h, byte s, byte v);   // HSV 255, 255, 255

---
Автор: Геннадий Дегтерёв  
gennadij@degterjow.de


* Основная страница оригинального проекта здесь https://alexgyver.ru/ledstairs/

