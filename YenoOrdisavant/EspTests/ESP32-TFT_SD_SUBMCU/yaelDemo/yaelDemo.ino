/**
 * ESP32 working w/ arduino IDE
 * 
 * Additional board url : https://dl.espressif.com/dl/package_esp32_index.json
 * goto Board Manager
 * choose ESP32 by espressif
 * 
 * DOIT ESP32 DEVKIT V1
 * 
 * for TFT-eSPI lib, in User_Setup.h, @EOF : add :: -- (now see TFT4inch Settings how-to)
 * // ===== Xtase Settings =====
 * #define TFT_MISO 19
 * #define TFT_MOSI 23
 * #define TFT_SCLK 18
 * #define TFT_CS    5  // Chip select control pin
 * #define TFT_DC   15  // Data Command control pin
 * //#define TFT_RST   4  // Reset pin (could connect to RST pin)
 * #define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
 * 
 * 
 * 1.3 MB Sktech
 * 320 KB RAM
 *
 *
 * FullDemo#2
 * 
 * (Sept 2109) - w/ RPI 4inch TFT Display
 * ESP32 + ILI9486 Screen + SDCard + MCP23017 Keyboard decoder
 * TFT_CS -> 5
 * TS_CS  -> 2
 * SD_CS  -> 4
 * 
 * SubMCU Bridge on RX2/TX2
 * 
 */

//====================================================================================
//                                    Settings
//====================================================================================

#define MODE_4INCH 1
#if MODE_4INCH
 #define TFT_WIDTH 480
 #define TFT_HEIGHT 320
 #define DEFAULT_ROTATION 3
#else
 #define TFT_WIDTH 320
 #define TFT_HEIGHT 240
 #define DEFAULT_ROTATION 1 
#endif


#include <HardwareSerial.h>
//====================================================================================
//                                    MP3 Player
//====================================================================================
HardwareSerial Serial3(1); // use uart3
#define mp3Serial Serial3

#include "DFPlayer_Simple.h"
SoundCard sndCard( &mp3Serial );

void setupMp3() {
    Serial3.begin(9600, SERIAL_8N1, 26, 14); // pins 26 rxY, 14 txY, 9600 bps, 8 bits no parity 1 stop bit
    sndCard.init();
    delay(500);
    sndCard.volume(25);
}

//====================================================================================
//                                    MCU Bridge
//====================================================================================

// HardwareSerial Serial2(2); // use uart2

void cleanBridge() {
    while( Serial2.available() ) {
        Serial2.read();
    }
}

// forward symbol
void cleanKeyb();
void led(bool state);

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

void led(bool state) {
    if ( state ) { Serial2.write('L'); }
    else  { Serial2.write('l'); }
    delay(1);
}

void drive_led(bool state) {
    led(state);
}

//====================================================================================
//                                    Keyboard
//====================================================================================

#define KB_BUFF_LEN 20
char keyBuff[KB_BUFF_LEN + 1];
bool firstKeyUse = true;

long lastKeyTime = millis();

void cleanKeyb() {
    Serial2.write((uint8_t)'K');
    delay(2);
}

char pollKeyb() {

    if ( firstKeyUse ) {
        firstKeyUse = false;
        memset(keyBuff, 0x00, KB_BUFF_LEN+1);
    }

    int tlen = strlen( keyBuff );
    if ( tlen == 0 && ( millis() - lastKeyTime > 60 ) ) {
      Serial2.write( (uint8_t)'k');  
      delay(4);
      lastKeyTime = millis();
    } 
    
    if ( tlen < KB_BUFF_LEN ) {
        while( Serial2.available() ) {
            keyBuff[ tlen++ ] = (char)Serial2.read();
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

// //====================================================================================
// //                                    LCD 20x4
// //====================================================================================

void lcd_clear() {
    Serial2.write( (uint8_t)'C' );

    delay(2);
}

void lcd_setCursor(int col, int row) {
    Serial2.write( (uint8_t)'c' );
    Serial2.write( (uint8_t)col );
    Serial2.write( (uint8_t)row );
    delay(2);
}

void lcd_home() {
    lcd_setCursor(0,0);
    delay(2);
}

void lcd_print(char* str) {
    Serial2.write( (uint8_t)'P' );
    Serial2.print( str );
    Serial2.write( (uint8_t)'\n' );
    delay(2);
}


// can be TX only ...
// HardwareSerial Serial1(1);
// Serial1.begin(9600, SERIAL_7E1, 12, -1, true); ....

//====================================================================================
//                                  Libraries
//====================================================================================

#include "FS.h"
#include "SD.h"
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
#define SD_CS 4 // SD chip select

#include "drawPAK.h"
#include "drawBMP.h"


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  // Serial.begin(115200); // Used for messages and the C array generator
  Serial.begin(9600); // Used for messages and the C array generator

  setupBridge();

  lcd_print("Init MP3\n");
  setupMp3();

  // Initialise the SD library before the TFT so the chip select gets set
  lcd_print("Init SD\n");
  if (!SD.begin(SD_CS)) {
    Serial.println("Initialisation failed!");
    lcd_print("! SD failed !");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  // Now initialise the TFT
  lcd_print("Init TFT\n");
  tft.begin();
  tft.setRotation(DEFAULT_ROTATION);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  // Draw Wallpaper
  tft.setRotation(DEFAULT_ROTATION == 1 ? 2 : 0);
  tft.fillScreen(random(0xFFFF));
  drawBmp("/Z/0/GIRL.BMP", 0, 0);
  tft.setRotation(DEFAULT_ROTATION);
  drawImgFromPAK("/z/0/game1-1.pak", 0, 0, 6);

  tft.println("Screen OK");
  tft.println("SDCard OK");
  tft.println("Checking Keyboard : ");

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

}

//====================================================================================
//                                    Loop
//====================================================================================
int numImg = 0;

#define SAMPLE_LEN 100
long times[SAMPLE_LEN];
long maxT = 0;
long minT = 10000;
long t0,t1,t,avg;

int loopCpt = -1;

bool firstKeyPressed = true;

void loop()
{
  if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
    cleanBridge();
  }


  // tft.fillScreen(random(0xFFFF));
  // // drawBmp("/parrot.bmp", 0, 0);//, 16);
  // drawBmp("/Z/0/GIRL.BMP", 0, 0);

  // tft.setRotation(1);
  // drawImgFromPAK("/z/0/game1-1.pak", 0, 0, numImg);

  // if ( numImg++ >= 5 ) { numImg=0; } 
  // delay(4000);

  t0 = millis();
  char key = pollKeyb();

  // current Space keySymbol
  if ( key == 'z' ) {
      if ( sndCard.isPlaying() ) { sndCard.stop(); }
      else { sndCard.play(1); }
  }

  t1 = millis();
  t = t1 - t0;
  if ( t > maxT ) { maxT = t; }
  if ( t < minT ) { minT = t; }
  times[ loopCpt ] = t;

  loopCpt++;
  if ( loopCpt >= SAMPLE_LEN ) {
    loopCpt = 0;
    // avg = 0;
    // for(int i=0; i < SAMPLE_LEN; i++) {
    //   avg += times[i];
    // }
    // avg = (long) ((double)avg / (double)SAMPLE_LEN);

    // char msg[20+1]; memset(msg, 0x00, 20);

    // lcd_clear(); lcd_home(); memset(msg, 0x00, 20);
    // sprintf(msg, "AVG:%lu", avg); // %ld -> long signed / %lu long unsigned
    // lcd_print(msg); lcd_setCursor(0, 1); memset(msg, 0x00, 20);
    // sprintf(msg, "MIN:%lu", minT);
    // lcd_print(msg); lcd_setCursor(0, 2);  memset(msg, 0x00, 20);
    // sprintf(msg, "MAX:%lu", maxT);
    // lcd_print(msg); lcd_setCursor(0, 3);  memset(msg, 0x00, 20);

    // minT = 10000;
    // maxT = 0;
  }

  if ( key != 0x00 ) {
      tft.print( (char)key );

      if (firstKeyPressed) {
          lcd_clear();
          //         12345678901234567890
          lcd_print("Keyboard works !");
          firstKeyPressed = false;
      }
  }

  delay( 10 );

}
//====================================================================================

