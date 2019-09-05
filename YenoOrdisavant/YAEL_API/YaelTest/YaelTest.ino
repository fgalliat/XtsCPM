/**
 * YAEL API Test
 * 
 * Xtase - fgalliat @Sept2019
 * 
 *  
 */
#include "Yael.h"

void setup()
{
  bool ok = yael_setup();
  if ( !ok ) {
      yael_lcd_cls();
      yael_lcd_setCursor(1,1);
      yael_lcd_print("Something Wrong !");
      while (1) {
          yield();
      }
  }

  // Draw Wallpaper
  yael_tft_drawBMP("/Z/0/GIRL.BMP", 0, 0);
  yael_tft_drawPAK("/z/0/game1-1.pak", 480-320, 320-240, 6);
  yael_tft_drawPAK("/z/0/ishar.pak", 0, 0, 2);

  yael_tft_println("Screen OK");
  yael_tft_println("SDCard OK");
  yael_tft_println("Checking Keyboard : (z to play MP3)");

  // aux screen
  yael_lcd_setCursor(0,0);
  yael_lcd_print("== Xtase @Aug2019 ==");
  yael_lcd_setCursor(0,1);
  //         12345678901234567890
  yael_lcd_print("OrdiSavant new YATL");
  yael_lcd_setCursor(0,2);
  yael_lcd_print("Layout...");
  yael_lcd_setCursor(0,3);
  yael_lcd_print("Have fun !");
}

int loopCpt = -1;
bool firstKeyPressed = true;

void loop() {
    if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
    // cleanBridge();
  }

  char key = yael_keyb_poll();

  // current Space keySymbol
  if ( key == 'z' ) {
      if ( yael_mp3IsPlaying() ) { yael_mp3Stop(); }
      else { yael_mp3Play(1); }
  }

  if ( key != 0x00 ) {
      yael_tft_print( (char)key );

      if (firstKeyPressed) {
          yael_lcd_cls();
          //         12345678901234567890
          yael_lcd_print("Keyboard works !");
          firstKeyPressed = false;
      }
  }

  delay( 10 );

}
