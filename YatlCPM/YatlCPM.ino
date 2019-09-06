#include "globals.h"

#include "xts_string.h"

#include <SPI.h>

// keep as custom entry point
#ifdef XTASE_T36_YATL_LAYOUT
  #if defined ESP32
    // but redef. right layout
    #undef XTASE_T36_YATL_LAYOUT
    #define XTASE_ESP_YATL_LAYOUT 1
    #define YAEL_PLATFORM 1
    #warning "BEWARE : it's a YAEL config"

    bool Serial_useable = true;

    // TODO
    #define TFT_CS 5
    #define TS_CS 2
    #define SD_CS 4

/*

Stack smashing protect failure!

abort() was called at PC 0x400e51e4 on core 1

Backtrace: 0x400893b4:0x3ffb1d80 0x400895e1:0x3ffb1da0 0x400e51e4:0x3ffb1dc0 0x400d1db7:0x3ffb1de0 0x400d1e1d:0x3ffb1e50 0x400d21cd:0x3ffb1e90 0x400d21f9:0x3ffb1eb0 0x400d2bc2:0x3ffb1ed0 0x400d2d9b:0x3ffb1f20 0x400d544d:0x3ffb1f40 0x400dc123:0x3ffb1fb0 0x4008561d:0x3ffb1fd0

Decoding stack results
0x400893b4: invoke_abort at /Users/ficeto/Desktop/ESP32/ESP32/esp-idf-public/components/esp32/panic.c line 155
0x400895e1: abort at /Users/ficeto/Desktop/ESP32/ESP32/esp-idf-public/components/esp32/panic.c line 170
0x400e51e4: __stack_chk_fail at /Users/ficeto/Desktop/ESP32/ESP32/esp-idf-public/components/esp32/stack_check.c line 36
0x400d1db7: _findnext(unsigned char) at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/abstraction_arduino_esp.h line 391
0x400d1e1d: _findfirst(unsigned char) at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/abstraction_arduino_esp.h line 401
0x400d21cd: _SearchFirst(unsigned short, unsigned char) at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/disk.h line 286
0x400d21f9: _CheckSUB() at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/disk.h line 507
0x400d2bc2: _Bdos() at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/cpm.h line 545
0x400d2d9b: cpu_in(unsigned int) at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/cpu.h line 34
0x400d544d: setup() at C:\Users\Franck\AppData\Local\Temp\arduino_build_502086\sketch/cpu.h line 2765
0x400dc123: loopTask(void*) at C:\Users\Franck\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.2\cores\esp32\main.cpp line 14
0x4008561d: vPortTaskWrapper at /Users/ficeto/Desktop/ESP32/ESP32/esp-idf-public/components/freertos/port.c line 143


 */



    #define SDINIT SD_CS
  #endif
#endif


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
  #include <SD.h>
#else
  #include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
#endif

// Serial port speed
#ifdef YATL_PLATFORM
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
  #else

    #if YAEL_PLATFORM
      // TODO : disable Screen & TouchScreen SPI C-select
      pinMode(TFT_CS, OUTPUT);
      digitalWrite(TFT_CS, HIGH);
      pinMode(TS_CS, OUTPUT);
      digitalWrite(TS_CS, HIGH);
      // enable SD card C-select
      pinMode(SD_CS, OUTPUT);
      digitalWrite(TS_CS, LOW);
    #endif

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
  #ifdef YATL_PLATFORM
  // do nothing here
  if (true) {
  #else
  // all other platform
  if (SD.begin(SDINIT)) {
  #endif
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
  #ifdef YATL_PLATFORM
    yatl.blink(3);
    delay(500);
  #else
    digitalWrite(LED, HIGH^LEDinv);
    delay(DELAY);
    digitalWrite(LED, LOW^LEDinv);
    delay(DELAY);
    digitalWrite(LED, HIGH^LEDinv);
    delay(DELAY);
    digitalWrite(LED, LOW^LEDinv);
    delay(DELAY * 4);
  #endif
}
