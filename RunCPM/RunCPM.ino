#include "globals.h"



#include <SPI.h>

#ifdef XTASE_T36_YATL_LAYOUT
  // auto-provides an 'SD' instance
  // this pseudo include is just used to specify
  // (..) the wanted impl. of lib
  // #include <SD_xts.h>

  // regular teensy SD lib
  // #include <SD.h>

  // maybe slower but as direct truncate, rename, .... routines that are required by the system
  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager

  #include "xts_yatl.h"

  #ifdef HAS_BUILTIN_BUZZER
    #define USE_BUILTIN_BUZZER 1
    #include "xts_yatl_speaker.h"
    
    void beep() { buzzer.beep(); }
  #else
    void beep() { ; }
  #endif

  #ifdef HAS_BUILTIN_LCD
    #define USE_BUILTIN_LCD 1
    #define BOOT_BUILTIN_LCD 1

    #include "xts_yatl_screen.h"
  #endif

#else
  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
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
  SdFatSoftSpiEX<2, 15, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26
  #define SDINIT 13 // CS
  #define LED 22 // TTGO_T1=22 LOLIN32_Pro=5(inverted) DOIT_Esp32=2 ESP32-PICO-KIT=no led
  #define LEDinv 0
  #define BOARD "TTGO_T1"
#elif defined CORE_TEENSY // Teensy 3.5 and 3.6
  #ifdef XTASE_T36_YATL_LAYOUT
    // // SD instance is auto-provided by include "SD.h"

    // w/ SdFat lib on a Teensy 3.5/3.6,
    // (..) SD.begin( BUILTIN_SDCARD ) becomes
    // (..) SD.begin( )
    SdFatSdio SD;
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

#include "abstraction_arduino.h"

#ifdef ESP32        // ESP32 specific CP/M BDOS call routines
  #include "esp32.h"
#endif
#ifdef _STM32_DEF_  // STM32 specific CP/M BDOS call routines
  #include "stm32.h"
#endif

// Serial port speed
#define SERIALSPD 9600

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
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(SERIALSPD);

  #ifdef USE_BUILTIN_LCD
    #warning "-USE BUILTIN LCD-"
    setupArduinoScreen();
    #ifdef BOOT_BUILTIN_LCD
      #warning "-BOOT ON BUILTIN LCD-"

      #ifdef LCD_MODE_CONSOLE
      //  #define CONSOLE_SELF_TEST 1
       #ifdef CONSOLE_SELF_TEST
        _consoleTest();
        while( true ) {
          delay(3000);
        }
       #endif
      #endif

    #endif
  #endif

  #ifdef USE_BUILTIN_BUZZER
    #warning "- BOOT W/ BUILTIN SPEAKER -"
    setupArduinoSpeaker();
  #endif

  #ifndef BOOT_BUILTIN_LCD
    while (!Serial) {	// Wait until serial is connected
      digitalWrite(LED, HIGH^LEDinv);
      delay(sDELAY);
      digitalWrite(LED, LOW^LEDinv);
      delay(sDELAY);
    }
  #endif

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

  _puts("Initializing SD card.\r\n");
#if defined ADAFRUIT_GRAND_CENTRAL_M4
  if (SD.cardBegin(SDINIT, SD_SCK_MHZ(50))) {

    if (!SD.fsBegin()) {
      _puts("\nFile System initialization failed.\n");
      return;
    }
#else
  if (SD.begin(SDINIT)) {
#endif
    if (VersionCCP >= 0x10 || SD.exists(CCPname)) {
      while (true) {
        _puts(CCPHEAD);
        _PatchCPM();
	Status = 0;
#ifndef CCP_INTERNAL
        if (!_RamLoad((char *)CCPname, CCPaddr)) {
          _puts("Unable to load the CCP.\r\nCPU halted.\r\n");
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
      _puts("Unable to load CP/M CCP.\r\nCPU halted.\r\n");
    }
  } else {
    _puts("Unable to initialize SD card.\r\nCPU halted.\r\n");
  }
}

void loop(void) {
  digitalWrite(LED, HIGH^LEDinv);
  delay(DELAY);
  digitalWrite(LED, LOW^LEDinv);
  delay(DELAY);
  digitalWrite(LED, HIGH^LEDinv);
  delay(DELAY);
  digitalWrite(LED, LOW^LEDinv);
  delay(DELAY * 4);
}
