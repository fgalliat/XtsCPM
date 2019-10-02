#include "globals.h"

#include "xts_string.h"

#include <SPI.h>




#ifdef XTASE_T36_YATL_LAYOUT
  // maybe slower but as direct truncate, rename, .... routines that are required by the system
  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager

  bool keybLocked = false;

  // needed here ....
  extern SdFatSdio SD;

  #include "xts_yatl_api.h"
  Yatl yatl;
  bool Serial_useable = false;

  // see if keep that file later ...
  #include "xts_yatl.h"


  #include "xts_yatl_bdos.h"

#elif defined XTASE_ESP_YATL_LAYOUT
  // changed lib @10/09/19
  //#include <SD.h>

  #include "xts_yael.h"
  extern SdFatSoftSpiEX<2, 4, 14> SD;

  bool Serial_useable = true;

  #define HAS_BUILTIN_LCD 0
  #define USE_BUILTIN_LCD 0
  // #define HAS_KEYBOARD 1

  #define SDINIT

  #include "xts_yael_bdos.h"

#else
  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
#endif

// Serial port speed
#if YATL_PLATFORM || YAEL_PLATFORM
  #define SERIALSPD 115200
#else
  #define SERIALSPD 9600
#endif



// SDCard/LED related definitions
//
//   SdFatSoftSpiEX and SdFatEX require changes to the following lines on SdFatConfig.h:
//     #define ENABLE_EXTENDED_TRANSFER_CLASS (from 0 to 1 - around line 71)
//     #define ENABLE_SOFTWARE_SPI_CLASS (from 0 to 1 - around line 87)
//
#if defined _STM32_DEF_ // STM32 boards
  SdFatSoftSpiEX<PC8, PD2, PC12> SD; // MISO, MOSI, SCK
  #define SDINIT PC11 // CS
  #define LED PD13
  #define LEDinv 0 // 0=normal 1=inverted
  #define BOARD "STM32F407DISC1"
#elif defined ESP32 // ESP32 boards

  #if YAEL_PLATFORM
    #define LED -1
    #define LEDinv 0
    #define BOARD "ESP32"
  #else
    SdFatSoftSpiEX<2, 15, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26
    #define SDINIT 13 // CS
    #define LED 22 // TTGO_T1=22 LOLIN32_Pro=5(inverted) DOIT_Esp32=2 ESP32-PICO-KIT=no led
    #define LEDinv 0
    #define BOARD "TTGO_T1"
  #endif
#elif defined CORE_TEENSY // Teensy 3.5 and 3.6
  #ifdef XTASE_T36_YATL_LAYOUT
    // // SD instance is auto-provided by include "SD.h"

    // w/ SdFat lib on a Teensy 3.5/3.6,
    // (..) SD.begin( BUILTIN_SDCARD ) becomes
    // (..) SD.begin( )
    
    // done upper ......
    // SdFatSdio SD;

    #define SDINIT
    
    // due to YATL SPI conection layout
    // #define LED 13
    #define LED 14
    #define LEDinv 0    
    #define BOARD "TEENSY 3.6"
  #else
    SdFatSdio SD;
    #define SDINIT
    #define LED 13
    #define LEDinv 0
    #define BOARD "TEENSY 3.5"
  #endif
#elif defined ADAFRUIT_GRAND_CENTRAL_M4
  #define USE_SDIO 0
  SdFat SD;
  #define SDINIT SDCARD_SS_PIN
  #define LED 13
  #define LEDinv 0
  #define BOARD "ADAFRUIT GRAND CENTRAL M4"
#else // Arduino DUE
  SdFatEX SD;
  #define SDINIT 4
  #define LED 13
  #define LEDinv 0
  #define BOARD "ARDUINO DUE"
#endif

// Delays for LED blinking
#define sDELAY 50
#define DELAY 100

#ifdef ESP32
  #include "abstraction_arduino_esp.h"
#else
  #include "abstraction_arduino.h"
#endif

#ifdef ESP32        // ESP32 specific CP/M BDOS call routines
  #include "esp32.h"
#endif
#ifdef _STM32_DEF_  // STM32 specific CP/M BDOS call routines
  #include "stm32.h"
#endif


// PUN: device configuration
#ifdef USE_PUN
File pun_dev;
int pun_open = FALSE;
#endif

// LST: device configuration
#ifdef USE_LST
File lst_dev;
int lst_open = FALSE;
#endif

#include "ram.h"
#include "console.h"
#include "cpu.h"
#include "disk.h"
#include "host.h"
#include "cpm.h"
#ifdef CCP_INTERNAL
#include "ccp.h"
#endif

void setup(void) {
  // pinMode(LED, OUTPUT);
  // digitalWrite(LED, LOW);
  // Serial.begin(SERIALSPD);

  #if YATL_PLATFORM
    setupYatl();
  #elif YAEL_PLATFORM
    bool ok = yael_setup();
    if ( !ok ) {
        yael_lcd_cls();
        yael_lcd_setCursor(1,1);
        yael_lcd_print("  Something Wrong !  ");
        while (1) {
            yael_lcd_setCursor(1,1);
            yael_lcd_print("  Something Wrong !  ");
            delay(500);
            yield();
        }
    }
  #else
    if ( LED > 0 ) {
      pinMode(LED, OUTPUT);
      digitalWrite(LED, LOW);
    }
    Serial.begin(SERIALSPD);
  #endif

  // SerialPort is useable or not .....
  Serial_useable = !(!Serial);

#ifdef DEBUGLOG
  _sys_deletefile((uint8 *)LogName);
#endif

  _clrscr();
  _puts("CP/M 2.2 Emulator v" VERSION " by Marcelo Dantas\r\n");
  _puts("Arduino read/write support by Krzysztof Klis\r\n");
  _puts("      Build " __DATE__ " - " __TIME__ "\r\n");
  _puts("--------------------------------------------\r\n");
  _puts("CCP: " CCPname "    CCP Address: 0x");
  _puthex16(CCPaddr);
  _puts("\r\nBOARD: ");
  _puts(BOARD);
  _puts("\r\n");

  char ccpFileName[13+1];
  memset( ccpFileName, 0x00, 13+1 );
  #if YAEL_PLATFORM
    // SD.h needs leading "/"
    sprintf(ccpFileName, "/%s", CCPname);
  #else
    sprintf(ccpFileName, "%s", CCPname);
  #endif


  _puts("Initializing SD card.\r\n");
#if defined ADAFRUIT_GRAND_CENTRAL_M4
  if (SD.cardBegin(SDINIT, SD_SCK_MHZ(50))) {

    if (!SD.fsBegin()) {
      _puts("\nFile System initialization failed.\n");
      return;
    }
#else
  #if YATL_PLATFORM || YAEL_PLATFORM
  // do nothing here
  if (true) {
  #else
  // all other platform
  if (SD.begin(SDINIT)) {
  #endif
#endif

#if YAEL_PLATFORM
    if ( yael_subMcuIsReady() ) {
      yael_lcd_cls();
      //              12345678901234567890
      yael_lcd_println("Welcome @ Xtase YAEL", 0);
      yael_lcd_println("Running CP/M 2.2    ", 1);
      yael_lcd_println("On ESP32 Board      ", 2);
      yael_lcd_println("@Oct 2019 - Xtase", 3);
    }
#endif


    // if (VersionCCP >= 0x10 || SD.exists(CCPname)) {
    if (VersionCCP >= 0x10 || SD.exists(ccpFileName)) {
      while (true) {
        _puts(CCPHEAD);
        _PatchCPM();
	Status = 0;
#ifndef CCP_INTERNAL
        // if (!_RamLoad((char *)CCPname, CCPaddr)) {
        if (!_RamLoad((char *)ccpFileName, CCPaddr)) {
          _puts("Unable to load the CCP [0x01].\r\nCPU halted.\r\n");
          break;
        }
        Z80reset();
        SET_LOW_REGISTER(BC, _RamRead(0x0004));
        PC = CCPaddr;
        Z80run();
#else
        _ccp();
#endif
        if (Status == 1)
          break;
#ifdef USE_PUN
        if (pun_dev)
          _sys_fflush(pun_dev);
#endif
#ifdef USE_LST
        if (lst_dev)
          _sys_fflush(lst_dev);
#endif

      }
    } else {
      _puts("Unable to load CP/M CCP [0x02].\r\nCPU halted.\r\n");
    }
  } else {
    _puts("Unable to initialize SD card [0x03].\r\nCPU halted.\r\n");
  }
}

void loop(void) {
  #if YATL_PLATFORM
    yatl.blink(3);
    delay(500);
  #elif YAEL_PLATFORM
    yael_led(true);
    delay(DELAY);
    yael_led(false);
    delay(DELAY);
    yael_led(true);
    delay(DELAY);
    yael_led(false);
    delay(DELAY);
  #else
    if ( LED > 0 ) {
      digitalWrite(LED, HIGH^LEDinv);
      delay(DELAY);
      digitalWrite(LED, LOW^LEDinv);
      delay(DELAY);
      digitalWrite(LED, HIGH^LEDinv);
      delay(DELAY);
      digitalWrite(LED, LOW^LEDinv);
    }
    delay(DELAY * 4);
  #endif
}
