/**
 * Xtase - fgalliat @Sept2019
 * 
 * YAEL routines Impl.
 * 
 * ESP32 working w/ arduino IDE
 * 
 * Additional board url : https://dl.espressif.com/dl/package_esp32_index.json
 * goto Board Manager
 * choose ESP32 by espressif
 * 
 * DOIT ESP32 DEVKIT V1
 * 
 * for TFT-eSPI lib, (now see TFT4inch Settings how-to...)
 * ESP32 + ILI9486 Screen + SDCard + MCP23017 Keyboard decoder
 * // ===== Xtase Settings =====
 * #define TFT_MISO 19
 * #define TFT_MOSI 23
 * #define TFT_SCLK 18
 * #define TFT_CS    5  // Chip select control pin
 * #define TFT_DC   15  // Data Command control pin
 * #define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
 * 
 * #define SD_CS     4  // SdCard CHIP-SELECT
 * #define TS_CS     2  // TouchScreen CHIP-SELECT
 *
 * #define SD_CS     26 // REDEF for now
 * in SdFatConfig.h (of SdFatLib)
 *  If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
 *  will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
 *  the class SdFatSoftSpiEX will be defined.
 *  These classes used extended multi-block SD I/O for better performance.
 *  the SPI bus may not be shared with other devices in this mode.
 * 
 * 1.3 MB Sktech
 * 320 KB RAM
 *
 * SubMCU Bridge on RX2/TX2 (ProMini 328P 3.3v)
 * 
 * --------------------------
 * part of XtsCPM project
 */

#include "Arduino.h"

#include "xts_yael.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



#include <HardwareSerial.h>
//====================================================================================
//                                    AUX Serial Port
//====================================================================================
HardwareSerial Serial3(1); // use uart3
#define auxSerial Serial3

// 16,17 is RX2,TX2
#define AUX_IN  32
#define AUX_OUT 33

void setupAuxPort() {
    Serial3.begin(9600, SERIAL_8N1, AUX_IN, AUX_OUT); // pins 26 rxY, 14 txY, 9600 bps, 8 bits no parity 1 stop bit

    // Serial3.println("Hello Serial3 !");
    // Serial.println("Hello Serial3 !");

    // Serial.println("Reading Serial3 !");
    // char line[32];
    // while( Serial3.available() == 0 ) { delay(100); }
    // int nb = Serial3.readBytesUntil('\n', line, 32);
    // Serial3.println(line);
    // Serial.println(line);
}

//====================================================================================
//                                    MCU Bridge
//====================================================================================

// HardwareSerial Serial2(2); // use uart2
// can be TX only ...
// HardwareSerial Serial1(1);
// Serial1.begin(9600, SERIAL_7E1, 12, -1, true); ....

#define bridgeSerial Serial2

void cleanBridge() {
    while( Serial2.available() ) {
        Serial2.read();
    }
}

// forward symbol
void cleanKeyb();
void led(bool state, bool fastMode=false);

void setupBridge() {
    // Serial2.begin(9600, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 9600 bps, 8 bits no parity 1 stop bit
    Serial2.begin(9600);
    cleanBridge();
    led(true);
    cleanKeyb();
    led(false);
}

//====================================================================================
//                                    Led
//====================================================================================

void led(bool state, bool fastMode) {
    if ( state ) { bridgeSerial.write('L'); }
    else  { bridgeSerial.write('l'); }
    if (!fastMode) delay(1);
}

void drive_led(bool state) {
    led(state, true);
}

//====================================================================================
//                                    Keyboard
//====================================================================================

#define KB_BUFF_LEN 20
char keyBuff[KB_BUFF_LEN + 1];
bool firstKeyUse = true;

long lastKeyTime = millis();

void cleanKeyb() {
    bridgeSerial.write((uint8_t)'K');
    delay(2);
}

char pollKeyb() {

    if ( firstKeyUse ) {
        firstKeyUse = false;
        memset(keyBuff, 0x00, KB_BUFF_LEN+1);
    }

    int tlen = strlen( keyBuff );
    if ( tlen == 0 && ( millis() - lastKeyTime > 60 ) ) {
      bridgeSerial.write( (uint8_t)'k');  
      delay(4);
      lastKeyTime = millis();
    } 
    
    if ( tlen < KB_BUFF_LEN ) {
        while( bridgeSerial.available() ) {
            keyBuff[ tlen++ ] = (char)bridgeSerial.read();
            if ( tlen >= KB_BUFF_LEN ) {
                break;
            }
        }
        keyBuff[ tlen ] = 0x00;
    }

    char customKey = 0x00;

    if ( tlen > 0 ) {
        customKey = keyBuff[0];

        // memmove ....
        for(int i=1; i < tlen; i++) {
            keyBuff[i-1] = keyBuff[i];
        }
        keyBuff[ tlen-1 ] = 0x00;
        keyBuff[ tlen ] = 0x00;
    }

return customKey;
}

//====================================================================================
//                                    LCD 20x4
//====================================================================================

void lcd_clear() {
    bridgeSerial.write( (uint8_t)'C' );
    delay(2);
}

void lcd_setCursor(int col, int row) {
    bridgeSerial.write( (uint8_t)'c' );
    bridgeSerial.write( (uint8_t)col );
    bridgeSerial.write( (uint8_t)row );
    delay(2);
}

void lcd_home() {
    lcd_setCursor(0,0);
    delay(2);
}

void lcd_print(char* str) {
    bridgeSerial.write( (uint8_t)'P' );
    bridgeSerial.print( str );
    bridgeSerial.write( (uint8_t)'\n' );
    delay(2);
}

//====================================================================================
//                                    MP3 Player
//====================================================================================
void setupMp3() {
    // sndCard.init();
    // delay(500);
    // sndCard.volume(25);
}



//====================================================================================
//                                  Libraries
//====================================================================================

#include "SPI.h"

// Call up the TFT library
#include <TFT_eSPI.h> // Hardware-specific library for ESP8266

// Invoke TFT library this will set the TFT chip select high
TFT_eSPI tft = TFT_eSPI();



// ==== Wiring =====
#define OWN_SPI_CS   5
#define OWN_SPI_DC   15
#define OWN_SPI_MOSI 23
#define OWN_SPI_CLK  18
#define OWN_SPI_MISO 19

#define TFT_CS OWN_SPI_CS
#define TS_CS 26
#define SD_CS 13 // SD chip select

//                ___________
//            3.3|           |VIN
//            GND|           |GND
//  TFT_DC    D15|           |D13 SD_CS
//  SD_MISO   D02|           |D12
//  SD_MOSI   D04|           |D14 SD_CLK
//  Sub_MCU   RX2|           |D17
//  Sub_MCU   TX2|           |D26 TS_CS
//  TFT_CS    D05|           |D25
//  TFT_CLK   D18|           |D33 TX1 (aux ExtPort)
//  TFT_MISO  D19|           |D32 RX1 (aux ExtPort)
//            D21|           |D35
//            RX0|           |D34
//            TX0|           |VN
//            D22|           |VP
//  TFT_MOSI  D23|___________|EN /RESET

// #include "FS.h"
// #include "SD.h"

// in SdFatConfig.h (of SdFatLib)
//  * If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
//  * will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
//  * the class SdFatSoftSpiEX will be defined.
//  * These classes used extended multi-block SD I/O for better performance.
//  * the SPI bus may not be shared with other devices in this mode.

/*
rst:0x10 (RTCWDT_RTC_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)
flash read err, 1000
ets_main.c 371 
 */

#include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
// SdFatSoftSpiEX<12, 14, 27> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26
// SdFatSoftSpiEX<2, 15, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26

SdFatSoftSpiEX<2, 4, 14> SD; // MISO, MOSI, SCK Some boards use 2,15,14,13, other 12,14,27,26


// === Now that TFT & SD Loaded ... ===
#include "xts_yael_soft_drawBMP.h"
#include "xts_yael_soft_drawPAK.h"


void _setupCSlines() {
  // disable Screen & TouchScreen SPI C-select
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(TS_CS, OUTPUT);
  digitalWrite(TS_CS, HIGH);
  // enable SD card C-select
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, LOW);
}



// ========================================

bool Y_setup()
{
  _setupCSlines();

  Serial.begin(115200); // Used for messages and the C array generator
//   Serial.begin(9600); // Used for messages and the C array generator

  setupBridge();
  setupAuxPort();

  // Now initialise the TFT
  lcd_print("Init TFT\n");
  tft.begin();
  tft.setRotation(DEFAULT_TFT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  // Initialise the SD library before the TFT so the chip select gets set
  // have some issues w/ RESETing Screen ...
  lcd_print("Init SD\n");
  int retry = 1;
  bool ok = false;
  while( retry <= 3 ) {
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Initialisation failed!");
        lcd_print("! SD failed ! ");
        yael_tft_print("! SD failed ! ");
        // return false;
    } else {
        ok = true;
        break;
    }
    delay(300);
    _setupCSlines();
    yield();
    retry++;
  }
  if ( !ok ) {
      return false;
  }
  Serial.println("\r\nSD Initialisation done.");

  lcd_print("Init MP3\n");
  setupMp3();

//   // Now initialise the TFT
//   lcd_print("Init TFT\n");
//   tft.begin();
//   tft.setRotation(DEFAULT_TFT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
//   tft.fillScreen(TFT_BLACK);

  // aux screen
  lcd_setCursor(0,0);
  lcd_print("== Xtase @Aug2019 ==");
  lcd_setCursor(0,1);
  //         12345678901234567890
  lcd_print("OrdiSavant new YATL");
  lcd_setCursor(0,2);
  lcd_print("Layout...");
  lcd_setCursor(0,3);
  lcd_print("Have fun !");

return true;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$ YAEL API

bool yael_setup() { return Y_setup(); }

void yael_tft_cls() { tft.fillScreen(TFT_BLACK); tft.setCursor(0,0); }
void yael_tft_setCursor(int col, int row) { tft.setCursor(col,row); }
void yael_tft_print(char ch) { 
    if ( ch == 26 ) { yael_tft_cls(); return; }
    tft.print(ch); 
}
void yael_tft_print(char* str) { tft.print(str); }
void yael_tft_println(char* str) { tft.println(str); }
void yael_tft_drawBMP(char* filename, int x, int y) { 
    tft.setRotation(DEFAULT_TFT_ROTATION == 1 ? 2 : 0);
    drawBmp(filename, x, y);
    tft.setRotation(DEFAULT_TFT_ROTATION);
}
void yael_tft_drawPAK(char* filename, int x, int y, int imgNum) { drawImgFromPAK(filename, x, y, imgNum); }

void yael_lcd_cls() { lcd_clear(); }
void yael_lcd_setCursor(int col, int row) { lcd_setCursor(col,row); }
void yael_lcd_print(char* str) { lcd_print(str); }

bool _mp3Playing = false;

void yael_mp3Play(int trackNum) { 
    bridgeSerial.print("MP"); bridgeSerial.write( (char)(trackNum/256) ); bridgeSerial.write( (char)(trackNum%256) ); delay(2);
    _mp3Playing = true; 
}
void yael_mp3Loop(int trackNum) { yael_lcd_print( (char*)"(!!) MP3 LOOP NYI" ); }
void yael_mp3Vol(int volume) { bridgeSerial.print("MV"); bridgeSerial.write( (char)(volume%(30+1)) ); delay(2); }

void yael_mp3Pause() { bridgeSerial.print("Mp"); delay(2); _mp3Playing = !_mp3Playing;}
void yael_mp3Stop() { bridgeSerial.print("Ms"); delay(2); _mp3Playing = false; }
void yael_mp3Next() { bridgeSerial.print("Mn"); delay(2); _mp3Playing = true; }
void yael_mp3Prev() { bridgeSerial.print("Mv"); delay(2); _mp3Playing = true; }

// TODO : TEMP
bool yael_mp3IsPlaying() { return _mp3Playing; }


// void yael_mp3Play(int trackNum) { sndCard.play(trackNum); }
// void yael_mp3Loop(int trackNum) { yael_lcd_print( (char*)"(!!) MP3 LOOP NYI" ); }
// void yael_mp3Vol(int volume) { sndCard.volume( volume ); }
// void yael_mp3Pause() { sndCard.pause(); }
// void yael_mp3Stop() { sndCard.stop(); }
// void yael_mp3Next() { sndCard.next(); }
// void yael_mp3Prev() { sndCard.prev(); }
// bool yael_mp3IsPlaying() { return sndCard.isPlaying(); }

void yael_led(bool state, bool fastMode) { led(state, fastMode); }

char yael_keyb_poll() { return pollKeyb(); }

void yael_dbug(char* str) { 
    Serial.print("(ii) "); Serial.println(str); 
    yael_lcd_print(" (ii) ");
    yael_lcd_print(str);
    yael_lcd_print("\n");
}

void yael_dbug(const char* str) { yael_dbug( (char*)str ); }