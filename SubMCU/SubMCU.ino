/**
 * SubMCU - Lower Layout for YATL
 * 
 * Designed to handle
 *  - MS Chatpad as builtin keyboard [v]
 *  - builtin Joypad & BTNs
 *  - ESP01 as WiFi adapter
 *  - DFPlayer as MP3 Player
 *  - ?Battery Voltmeter?
 *  - ?RGB Led?
 *  - Bridge to Upper Layout
 * 
 * Xtase - fgalliat @May2019
 */

// #include "DFRobotDFPlayerMini.h"
// SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

#define HAS_MP3 1
#define HAS_KEYB 1

#ifdef HAS_MP3
 #include <DFRobotDFPlayerMini.h>
 DFRobotDFPlayerMini myDFPlayer;
 #define SerialMP3 Serial2
#endif

#ifdef HAS_KEYB
 #include "ChatpadInputStream.h"
 ChatpadInputStream keyboard0(Serial1);

 int _chatPadErrorCounter = 0;

 void _handleChatPadError(int value) {
    if ( value < 0 ) {
    //   Serial.print("CheckSum error");
    //   Serial.println("");
       return;
    }
    // Serial.print("Unexpected packet type: ");
    // Serial.println(value, HEX);
    led(true); delay(10);
    led(false); delay(10);

    _chatPadErrorCounter++;
    if ( _chatPadErrorCounter >= 10 ) {
        keyboard0.init(false);
    }
 }

#endif

#define LED 13
void led(bool state) { digitalWrite( LED, state ? HIGH : LOW ); }

// ===== Joypad Section ======
// 0..1023 analogRead
#define PAD_AXIS_X 14
#define PAD_AXIS_Y 15
// used in pullup mode
#define PAD_BTN_1 16
#define PAD_BTN_2 17

uint8_t joypadX = 127;
uint8_t joypadY = 127;
uint8_t joypadB1 = 0;
uint8_t joypadB2 = 0;

void setupJoypad() {
   pinMode( PAD_AXIS_X, INPUT );
   pinMode( PAD_AXIS_Y, INPUT );
   pinMode( PAD_BTN_1, INPUT_PULLUP );
   pinMode( PAD_BTN_2, INPUT_PULLUP );
}

void pollJoypad() {
    int x = analogRead(PAD_AXIS_X);
    int y = analogRead(PAD_AXIS_Y);

    // map x,y
    joypadX = map(x, 0, 1023, 0, 255);
    joypadY = map(y, 0, 1023, 0, 255);

    // beware INPUT_PULLUP
    joypadB1 = ( digitalRead(PAD_BTN_1) == LOW ) ? 1 : 0;
    joypadB2 = ( digitalRead(PAD_BTN_2) == LOW ) ? 1 : 0;
}

void debugJoypad() {
    Serial.print("> Pad : ");
    Serial.print(joypadX);
    Serial.print("\t");
    Serial.print(joypadY);
    Serial.print("\t");
    Serial.print(joypadB1);
    Serial.print(joypadB2);
    Serial.println("");
}


// ===== MCU Bridge Section == 
int _avail() { return Serial.available(); }
int _read() { return Serial.read(); }
void _send(char ch) { Serial.write(ch); }

void setup() {
   pinMode( LED, OUTPUT );
   digitalWrite( LED, LOW );

   setupJoypad();

   Serial.begin(115200);

   // + system info
   // > logging info

   // ===== Ms ChatPad for XBOX 360 =====
   #ifdef HAS_KEYB
    delay(600); // to let keyboard enough time to init...
    keyboard0.init();
    keyboard0.disableAutoPoll();
    delay(300); // to let keyboard enough time to init...
    Serial.println("+k:OK"); // keyboard is OK
   #else
    Serial.println("+k:NOK"); // keyboard is NOK
   #endif

   // ===== DFPlayer mini MP3 =====
   #ifdef HAS_MP3
    SerialMP3.begin(9600);
    if (!myDFPlayer.begin(SerialMP3)) {
        Serial.println(F("> Unable to begin:"));
        Serial.println(F("> 1.Please recheck the connection!"));
        Serial.println(F("> 2.Please insert the SD card!"));
        Serial.println("+m:NOK");
    }
    else {
        Serial.println("+m:OK");
        myDFPlayer.volume(20);  //Set volume value. From 0 to 30
        // myDFPlayer.play(1);  //Play the first mp3
    }
   #else
    Serial.println("+m:NOK");
   #endif




   led(true); delay(300);
   led(!true); delay(300);
   led(true); delay(300);
   led(!true); delay(300);
   Serial.println("> Ready to work");
}

int loopCounter = 0;

void loop() {
    pollJoypad();

    #ifdef HAS_KEYB
     keyboard0.poll();
    #endif

    if ( _avail() ) {
        char ch = _read();
        if ( ch == 'k' ) {
            #ifdef HAS_KEYB
                if ( !keyboard0.available() ) { 
                    // Serial.println("> <Empty Buffer>"); 
                    _send(0x00); 
                }
                else {
                    while( keyboard0.available() >= 1 ) { _send( keyboard0.read() ); }
                    _send(0x00);
                }
            #else
                _send(0x00);
            #endif
        } else if ( ch == 'm' ) {
            // MP3 subCommands to dispatch
            #ifdef HAS_MP3
              myDFPlayer.play(65); // THEC64-MENU-Theme
            #endif
        } else if ( ch == 'j' ) {
            debugJoypad();

            _send( joypadX );
            _send( joypadY );
            _send( joypadB1 );
            _send( joypadB2 );
        } else if ( ch == '\n' || ch == '\r' ) {
            // may be some dusty end of line due to terminal 
            // that was used
        } else {
            Serial.print("> Command ");
            Serial.print(ch);
            Serial.println(" is NYI ");
        }
    }


    loopCounter++;
    loopCounter %= 200;
    if ( loopCounter == 0 ) { 
        // Serial.print('.'); 
        #ifdef HAS_KEYB
          // due to a frequent read bug ...
          // keyboard0.init(false);
          // now done only if error handled
        #endif
    }
    delay(2);
}