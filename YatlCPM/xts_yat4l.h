/**
 * Xtase - fgalliat @Nov2019
 * 
 * YAT4L routines Headers
 * 
 * part of XtsCPM project
 */


//====================================================================================
//                                    Settings
//====================================================================================

	#define HAS_BUILTIN_LCD 1
	#define USE_BUILTIN_LCD 1
	#define HAS_KEYBOARD 0
	#undef HAS_KEYBOARD

#define MODE_4INCH 1
#if MODE_4INCH
 #define TFT_WIDTH 480
 #define TFT_HEIGHT 320
 #define DEFAULT_TFT_ROTATION 1
#else
 #define TFT_WIDTH 320
 #define TFT_HEIGHT 240
 #define DEFAULT_TFT_ROTATION 1 
#endif

//====================================================================================
//                     Pins
//====================================================================================

#define MP3_BUSY_PIN 3

#define SUBMCU_READY_PIN 4

#define LED_BUILTIN_PIN 5

//====================================================================================
  // Xtase run-time Handler
  #define USE_XTS_HDL 1
  // run-time handler function
  void xts_hdl();


//====================================================================================

  static uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) { return (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) );}

  static const uint16_t CLR_BLACK = rgb(0,0,0);
  static const uint16_t CLR_WHITE = rgb(255,255,255);
  static const uint16_t CLR_GREEN = rgb(0,255,0);
  static const uint16_t CLR_RED   = rgb(255,0,0);
  static const uint16_t CLR_BLUE  = rgb(0,0,255);
  static const uint16_t CLR_YELLOW = rgb(255,255,0);
  static const uint16_t CLR_MAGENTA = rgb(255,0,255);
  static const uint16_t CLR_CYAN = rgb(0,255,255);
  static const uint16_t CLR_ORANGE = rgb(255,165, 0);
  static const uint16_t CLR_PURPLE = rgb(123,0,123);
  static const uint16_t CLR_PINK = rgb(255,130,198);

//====================================================================================
//                                    Symbols
//====================================================================================
#include "Adafruit_ILI9486_Teensy.h" // Hardware-specific library for T3+
extern Adafruit_ILI9486_Teensy tft;

// Get the SdFat-Beta version
// Got it working on SPI1
#include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
// for SD.open(...)

//====================================================================================
//                                    API
//====================================================================================

bool yat4l_setup();

void yat4l_tft_cls();
void yat4l_tft_setCursor(int col, int row);
void yat4l_tft_print(char str);
void yat4l_tft_print(char* str);
void yat4l_tft_println(char* str);
void yat4l_tft_drawBMP(char* filename, int x, int y);
void yat4l_tft_drawPAK(char* filename, int x, int y, int imgNum);

void yat4l_tft_drawRect(int x, int y, int w, int h, uint16_t color);
void yat4l_tft_fillRect(int x, int y, int w, int h, uint16_t color);
void yat4l_tft_drawCircle(int x, int y, int radius, uint16_t color);
void yat4l_tft_fillCircle(int x, int y, int radius, uint16_t color);
void yat4l_tft_drawLine(int x, int y, int x2, int y2, uint16_t color);


void yat4l_lcd_cls();
void yat4l_lcd_setCursor(int col, int row);
void yat4l_lcd_print(char* str);
void yat4l_lcd_println(char* str, int row);

void yat4l_mp3Play(int trackNum);
void yat4l_mp3Loop(int trackNum);
void yat4l_mp3Vol(int volume);
void yat4l_mp3Pause();
void yat4l_mp3Stop();
void yat4l_mp3Next();
void yat4l_mp3Prev();
bool yat4l_mp3IsPlaying();

void yat4l_led(bool state, bool fastMode=false);

char yat4l_keyb_poll();

void yat4l_buzzer_init();
void yat4l_buzzer_tone(int freq, int duration);
void yat4l_buzzer_noTone();
void yat4l_buzzer_playTuneString(char* sequence);
bool yat4l_buzzer_playTuneFile(const char* tuneStreamName);
void yat4l_buzzer_beep(int freq=440, int duration=200);

// not concurent safe !
char* yat4l_fs_getAssetsFileEntry(char* assetName);
bool yat4l_fs_downloadFromSerial();

bool yat4l_subMcuIsReady();

// ==========================
char* yat4l_wifi_getIP();
char* yat4l_wifi_getSSID();

bool yat4l_wifi_close();
bool yat4l_wifi_beginAP();
bool yat4l_wifi_startTelnetd();

bool yat4l_wifi_loop();

void yat4l_wifi_telnetd_broadcast(char ch);
int  yat4l_wifi_telnetd_available();
int  yat4l_wifi_telnetd_read();
// ==========================

void yat4l_dbug(char* str);
void yat4l_dbug(const char* str);

void yat4l_warn(char* str);
void yat4l_warn(const char* str);