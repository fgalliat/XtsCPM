#ifndef YATL_SCREEN_H_
#define YATL_SCREEN_H_ 1

/**
 * Xtase YATL Screen interface
 * Xtase - fgalliat @Apr2019
 */

 class Screen {
     public:
       Screen() { ; }
       ~Screen() { ; }

       void cls() {
           y_dbug( "XTS: Screen cls" );
       }
       void println(char* str) {
           y_dbug( "XTS: ->" );
           y_dbug( str );
           y_dbug( "XTS: ->" );
       }
 };


 // declared in xts_yatl.h
 void setupArduinoScreen() {
      y_dbug("XTS: setupArduinoScreen -> TODO");
 }

 Screen screen;

#endif