#ifndef YATL_H_
#define YATL_H_ 1

/**
 * Xtase YATL Hardware interface
 * Xtase - fgalliat @Apr2019
 */

  #define YATL_PLATFORM 1

  #define HAS_BUILTIN_LCD 1

  // 13 is used by SPI LCD
  #define LED_PIN 14

  #define SPEAKER_PIN 6

  #define y_dbug(a) Serial.println(a)

  void setupArduinoScreen();

  // Xtase run-time Handler
  #define USE_XTS_HDL 1
  bool ledState = false;
  void xts_hdl() {
    // run-time handler function
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    ledState = !ledState;
  }

  #define HAS_BUILTIN_BUZZER 1
  void setupArduinoSpeaker();

  void setupISR();

  void setupYatl() {
    setupArduinoScreen();
    setupArduinoSpeaker();
    setupISR();
  }


  // @@@@@@@@@@@@@ ISR Routines @@@@@@@@@@@@@
  #include <TimerOne.h>

  bool inIRQ=false;
  int irqCpt=0;


  #define ISR_MODE_NONE        0
  #define ISR_MODE_PLAY_BUZZER 1

  volatile char buzzerTuneStr[ 256 ];
  void _doPlay(char* tune);

  // TODO : use real stacks !!!!
  uint8_t nextISR = ISR_MODE_NONE;
  void ISR_push(uint8_t nextISROpCode = ISR_MODE_NONE) {
    nextISR = nextISROpCode;
  }



  void timer_IRQ(){
    if (inIRQ) { return; }
    inIRQ = true;

    // if ( irqCpt > 1 ) {
    //     screenSync();
    // }
    if ( nextISR == ISR_MODE_NONE ) {
      // ...
    } else if ( nextISR == ISR_MODE_PLAY_BUZZER ) {
      Serial.println( "ISR PUSHED : ISR_MODE_PLAY_BUZZER" );
      nextISR = ISR_MODE_NONE;
      _doPlay( (char*)buzzerTuneStr );
    } else {
      // ...
    }

    irqCpt++;

    inIRQ = false;
  }


  void setupISR() {
    Timer1.initialize(350000); // 350ms 
    Timer1.attachInterrupt(timer_IRQ);
  }

  // =======================================

  // ex. MONKEY.T5K -> /Z/0/MONKEY.T5K
  // spe DISK for assets : "Z:"
  // 'cause CP/M supports ONLY up to P:

  const int _fullyQualifiedFileNameSize = 5 + (8+1+3) + 1;
  char _assetEntry[ _fullyQualifiedFileNameSize ];

  // not ThreadSafe !
  char* getAssetsFileEntry(char* assetName) {
    memset(_assetEntry, 0x00, _fullyQualifiedFileNameSize);
    strcpy( _assetEntry, "/Z/0/" );
    strcat( _assetEntry, assetName );
    return _assetEntry;
  }

  // =======================================
  extern void drawBmp(char* filename, bool screenRotate);

  void yatlSDinited(bool inited) {
    // drawBmp( getAssetsFileEntry( (char*)"girl.BMP"), true );
  }



  // =======================================

  // forward symbols
  void consoleColorSet(uint16_t bg, uint16_t fg, uint16_t acc);
  uint16_t rgb(uint8_t r, uint8_t g, uint8_t b);

  // I'm now able to send 255 long String to BDos Hook !!!!!
  void XtsBdosCall(uint8 regNum, int32 value) {
    if ( regNum == 225 ) {

      uint8_t *F = (uint8_t*)_RamSysAddr(value);
      char test[256];
      memset(test, 0x00, 256);

      // from TurboPascal 3 strings are 255 long max
      // & starts @ 1 ( 'cause @0 contains length)
      uint8_t len = F[0];
      memcpy(test, &F[1], len);

      Serial.println("/===== BDos String call =====\\");
      Serial.println(test);
      Serial.println("\\===== BDos String call =====/");

      drawBmp( getAssetsFileEntry( test ), true );
    } else if ( regNum == 226 ) {
      if ( value == 0 ) { 
        // to reset
        // consoleColorSet();

        /*
        // Color definitions
#define ILI9341_BLACK       0x0000  ///<   0,   0,   0
#define ILI9341_NAVY        0x000F  ///<   0,   0, 123
#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
#define ILI9341_RED         0xF800  ///< 255,   0,   0
#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define ILI9341_PINK 0xFC18 ///< 255, 130, 198
        */
        const uint16_t black = 0x0000;
        const uint16_t white = 0xFFFF;
        const uint16_t green = 0x07E0;

        consoleColorSet(black, white, green);
      } else {
        consoleColorSet( rgb(126, 155, 125), rgb(69,80,110), rgb(108-30,120-30,195-30) );
      }
    }
    
  }


#endif