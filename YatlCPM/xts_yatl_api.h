#ifndef _YATL_API_H_
#define _YATL_API_H_ 1

/**
 * Xtase - fgalliat @Jun2019
 * 
 * definition of Yatl hardware API
 * 
 * 
 */

#include "xts_yatl_dev_screen.h"

class Yatl;

class YatlSubMCU {
    private:
      Yatl* yatl;
    public:
      YatlSubMCU(Yatl* yatl) { this->yatl = yatl; }

      bool setup();
      void reboot(bool waitFor=true);

      void lock();
      void unlock();

      void println(const char* ch);

      void send(char ch);
      void send(const char* ch);
      void send(char* ch);

      void flush();
      int available();
      int read();
      char* readLine();
      char* waitForNonEmptyLine(unsigned long timeout=0);
      int readUntil(uint8_t until, char* dest, int maxLen);
      int readBytesUntil(uint8_t until, char* dest, int maxLen) { return readUntil(until, dest, maxLen); }
      int readBytes(char* dest, int maxLen);

      void cleanBuffer();
};

class YatlPWRManager {
    private:
      Yatl* yatl;
    public:
      YatlPWRManager(Yatl* yatl) { this->yatl = yatl; }
      float getVoltage();
      void reset(bool wholeSystem=true);
      void deepSleep(bool wholeSystem=true);
};

class YatlBuzzer {
    private:
      Yatl* yatl;
      bool _mute = false;
    public:
      YatlBuzzer(Yatl* yatl) { this->yatl = yatl; }
      void setup();

      void mute(bool _mute=true) { this->_mute = _mute; }

      void tone(int freq, int duration);
      void beep(int freq, int duration);
      void noTone();

      /**
       * plays a note or pseudo freq.
       * duration is a 1/50th
       */
      void playNote(int noteOrFreq, int duration);

      // ex. "AC#B"
      void playTuneString(const char* notes);
      // ex. "MONKEY.T5K"
      bool playTuneFile(const char* tuneFileName);
};

class YatlMusicPlayer {
    private:
      Yatl* yatl;
    public:
      YatlMusicPlayer(Yatl* yatl) { this->yatl = yatl; }
      void play(int trackNum);
      void loop(int trackNum);
      void stop();
      void pause();
      void next();
      void prev();
      void volume(uint8_t vol);
};

class YatlClock {
    private:
    public:
      YatlClock();
      long getTime();
      void setTime(long time);
};

class YatlFS {
    private:
      Yatl* yatl;
    public:
      YatlFS(Yatl* yatl) { this->yatl = yatl; }
      bool setup();
      char* getAssetsFileEntry(char* entryName);
      // Serial will have to send PATH/LEN/datas
      bool downloadFromSerial();
      bool downloadFromSubMcu();
};

class YatlWiFi {
    private:
      Yatl* yatl;
    public:
      YatlWiFi(Yatl* yatl) { this->yatl = yatl; }

      bool beginSTA();
      bool beginAP();
      void close();

      char* getIP();
      char* getSSID();

      bool startTelnetd();
      bool stopTelnetd();
      char* wget(char* url);
};

extern uint16_t rgb(uint8_t r,uint8_t g,uint8_t b); 
extern uint16_t mapColor(uint16_t color);

const uint16_t CLR_BLACK = rgb(0,0,0);
const uint16_t CLR_WHITE = rgb(255,255,255);
const uint16_t CLR_GREEN = rgb(0,255,0);
const uint16_t CLR_RED   = rgb(255,0,0);
const uint16_t CLR_BLUE  = rgb(0,0,255);

const uint16_t CLR_YELLOW = rgb(255,255,0);
const uint16_t CLR_MAGENTA = rgb(255,0,255);
const uint16_t CLR_CYAN = rgb(0,255,255);
const uint16_t CLR_ORANGE = rgb(255,165, 0);
const uint16_t CLR_PURPLE = rgb(123,0,123);
const uint16_t CLR_PINK = rgb(255,130,198);

// // Color definitions
// #define ILI9341_NAVY        0x000F  ///<   0,   0, 123
// #define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
// #define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
// #define ILI9341_MAROON      0x7800  ///< 123,   0,   0
// #define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
// #define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
// #define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
// #define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41


#define LCD_CONSOLE_40_COLS 0x00
#define LCD_CONSOLE_80_COLS 0x01
#define LCD_CONSOLE_DEF_COLS LCD_CONSOLE_80_COLS

class YatlScreen {
    private:
      Yatl* yatl;
    public:
      YatlScreen(Yatl* yatl) { this->yatl = yatl; }
      bool setup();

      void write(char ch);
      void println(const char* str);
      void println(char* str);

      void cls();

      void consoleColorSet(uint16_t bg=CLR_BLACK, uint16_t fg=CLR_WHITE, uint16_t acc=CLR_GREEN);
      void consoleSetMode(uint8_t columnMode, bool rerenderFull=true);

      void drawWallpaper(char* assetName);
      void drawTextBox(const char* title, const char* msg, uint16_t color=2);

      void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
      void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
      void drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
      void fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
      void drawLine(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, uint16_t color);

      void cleanSprites();
      void grabbSpritesOfSize(char* imageName, int offsetX=0, int offsetY=0, int width=32, int height=32);
      void grabbSprites(char* imageName, int offsetX=0, int offsetY=0);
};

class YatlJoypad {
    private:
    public:
      YatlJoypad();
};

class YatlKeyboard {
    private:
      Yatl* yatl;
    public:
      YatlKeyboard(Yatl* yatl) { this->yatl = yatl; }
      bool setup();
      void poll();
      int available(bool autopoll=true);
      int read();
};

class YatlLEDs {
    private:
      Yatl* yatl;
    public:
      YatlLEDs(Yatl* yatl) { this->yatl = yatl; }
      void red(bool state=true);
      void green(bool state=true);
      void blue(bool state=true);
      void builtin(bool state=true);
};


class Yatl {
    private:
      YatlSubMCU* subMcu;
      YatlBuzzer* buzzer;
      YatlPWRManager* pwrManager;
      YatlLEDs* leds;
      YatlWiFi* wifi;
      YatlMusicPlayer* mp3;
      YatlFS* fs;
      YatlKeyboard* keyb;
      YatlScreen* screen;
    public:
      Yatl();
      ~Yatl();

      // rather Serial DBUG
      void dbug(const char* str);
      void dbug(const char* str, int val);
      void dbug(const char* str, float val);

      void beep();

      // rather Screen (+ Serial)
      // for Immediate Native User Interaction
      void alert(const char* msg);
      void warn(const char* msg);
      bool confirm(const char* msg);

      bool setup();

      void delay(long time);
      void led(bool state);
      void blink(int times);

      // TODO : look at protected
      YatlSubMCU* getSubMCU() { return this->subMcu; }

      YatlPWRManager* getPWRManager() { return this->pwrManager; }

      YatlBuzzer* getBuzzer() { return this->buzzer; }
      YatlMusicPlayer* getMusicPlayer() { return this->mp3; }

      YatlClock* getClock();

      YatlLEDs* getLEDs() { return this->leds; }

      YatlScreen* getScreen() { return this->screen; }
      YatlKeyboard* getKeyboard() { return this->keyb; }
      YatlJoypad* getJoypad();

      YatlFS* getFS() { return this->fs; }

      YatlWiFi* getWiFi() { return this->wifi; }

};

#endif
