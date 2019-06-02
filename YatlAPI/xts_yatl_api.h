#ifndef _YATL_API_H_
#define _YATL_API_H_ 1

/**
 * Xtase - fgalliat @Jun2019
 * 
 * definition of Yatl hardware API
 * 
 * 
 */

class Yatl;

class YatlSubMCU {
    private:
      Yatl* yatl;
    public:
      YatlSubMCU(Yatl* yatl) { this->yatl = yatl; }

      bool setup();

      void send(char ch);
      void send(const char* ch);
      void send(char* ch);

      int read();
      char* readLine();
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
    public:
      YatlBuzzer(Yatl* yatl) { this->yatl = yatl; }
      void tone(int freq, int duration);
      void noTone();
};

class YatlMusicPlayer {
    private:
    public:
      YatlMusicPlayer();
      void play(int trackNum);
      void stop();
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
    public:
      YatlFS();
};

class YatlWiFi {
    private:
    public:
      YatlWiFi();
};

class YatlScreen {
    private:
    public:
      YatlScreen();
};

class YatlJoypad {
    private:
    public:
      YatlJoypad();
};

class YatlKeyboard {
    private:
    public:
      YatlKeyboard();
};

class YatlLEDs {
    private:
    public:
      YatlLEDs();
};


class Yatl {
    private:
      YatlSubMCU* subMcu;
      YatlBuzzer* buzzer;
    public:
      Yatl();
      ~Yatl();

      // rather Serial DBUG
      void dbug(const char* str);
      void dbug(const char* str, int val);
      void dbug(const char* str, float val);

      // rather Screen (+ Serial)
      // for Immediate Native User Interaction
      void alert(const char* msg);
      void warn(const char* msg);
      bool confirm(const char* msg);

      bool setup();

      void delay(long time);
      void led(bool state);
      void blink(int times);

      YatlPWRManager* getPWRManager();

      YatlBuzzer* getBuzzer();
      YatlMusicPlayer* getMusicPlayer();

      YatlClock* getClock();

      YatlLEDs* getLEDs();

      YatlScreen* getScreen();
      YatlKeyboard* getKeyboard();
      YatlJoypad* getJoypad();

      YatlFS* getFS();

      YatlWiFi* getWiFi();

};

#endif