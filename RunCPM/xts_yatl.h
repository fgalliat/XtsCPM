#ifndef YATL_H_
#define YATL_H_ 1

/**
 * Xtase YATL Hardware interface
 * Xtase - fgalliat @Apr2019
 */

  #define HAS_BUILTIN_LCD 1

  // 13 is used by SPI LCD
  #define LED_PIN 14

  #define y_dbug(a) Serial.println(a)

  void setupArduinoScreen();


#endif