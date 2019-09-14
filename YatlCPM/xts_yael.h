/**
 * Xtase - fgalliat @Sept2019
 * 
 * YAEL routines Headers
 * 
 * part of XtsCPM project
 */


//====================================================================================
//                                    Settings
//====================================================================================

#define MODE_4INCH 1
#if MODE_4INCH
 #define TFT_WIDTH 480
 #define TFT_HEIGHT 320
 #define DEFAULT_TFT_ROTATION 3
#else
 #define TFT_WIDTH 320
 #define TFT_HEIGHT 240
 #define DEFAULT_TFT_ROTATION 1 
#endif


//====================================================================================
//                                    Symbols
//====================================================================================
#include <TFT_eSPI.h> // Hardware-specific library for ESP8266
extern TFT_eSPI tft;

// #include "SD.h"
#include <SdFat.h>  // One SD library to rule them all - Greinman SdFat from Library Manager
// for SD.open(...)

//====================================================================================
//                                    API
//====================================================================================

bool yael_setup();

void yael_tft_cls();
void yael_tft_setCursor(int col, int row);
void yael_tft_print(char str);
void yael_tft_print(char* str);
void yael_tft_println(char* str);
void yael_tft_drawBMP(char* filename, int x, int y);
void yael_tft_drawPAK(char* filename, int x, int y, int imgNum);

void yael_lcd_cls();
void yael_lcd_setCursor(int col, int row);
void yael_lcd_print(char* str);

void yael_mp3Play(int trackNum);
void yael_mp3Loop(int trackNum);
void yael_mp3Vol(int volume);
void yael_mp3Pause();
void yael_mp3Stop();
void yael_mp3Next();
void yael_mp3Prev();
bool yael_mp3IsPlaying();

void yael_led(bool state, bool fastMode=false);

char yael_keyb_poll();

// not concurent safe !
char* yael_fs_getAssetsFileEntry(char* assetName);

void yael_dbug(char* str);
void yael_dbug(const char* str);