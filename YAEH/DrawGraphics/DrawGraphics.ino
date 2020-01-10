#include <Wire.h>
#include <I2C_LCD.h>
I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51; //Device address configuration, the default value is 0x51.

//For detials of the function useage, please refer to "I2C_LCD User Manual". 
//You can download the "I2C_LCD User Manual" from I2C_LCD WIKI page: http://www.seeedstudio.com/wiki/I2C_LCD


void setup(void)
{
    Wire.begin();         //I2C controller initialization.
    // delay(4000); // boot logo time ....
    LCD.WorkingModeConf( (LCD_SwitchState)OFF, (LCD_SwitchState)OFF, (LCD_WorkingMode)WM_CharMode );
}

void loop(void)
{
    // LCD.CleanAll(WHITE);    //Clean the screen with black or white.
    // // delay(1000);            //Delay for 1s.
    // delay(10);

    // //Draw a black horizontal line.
    // //Prototype: void DrawHLineAt(startX, endX, y, color)
    // LCD.DrawHLineAt(0, 127, 20, BLACK);
    // // delay(2000);            //Delay for 2s.
    // delay(10);
    
    // //Draw a black vertical line.
    // //Prototype: void DrawVLineAt(startY, endY, x, color)
    // LCD.DrawVLineAt(0, 63, 20, BLACK);
    // // delay(2000);            //Delay for 2s.
    // delay(10);

    // //Draw a black dot.
    // //Prototype: void DrawDotAt(x, y, color)
    // LCD.DrawDotAt(63, 50, BLACK);
    // // delay(2000);            //Delay for 2s.
    // delay(10);

    // //Draw a black line between (0,0) to (127, 63).
    // //Prototype: void DrawLineAt(startX, endX, startY, endY, color)
    // LCD.DrawLineAt(0, 127, 0, 63, BLACK);
    // // delay(2000);            //Delay for 2s.
    // delay(10);

    // //Draw a black filled circle.
    // //Prototype: void DrawCircleAt(x, y, r, mode)
    // LCD.DrawCircleAt(63, 31, 30, BLACK_FILL);
    // // delay(2000);            //Delay for 2s.
    // delay(10);

    // //Draw a black filled rectangle.
    // //Prototype: void DrawRectangleAt(x, y, width, height, mode)
    // LCD.DrawRectangleAt(10, 15, 107, 33, BLACK_FILL);
    // delay(10);

// ----------------------------------------------

int refreshTime = 1000;
refreshTime = 5;

    LCD.CleanAll(WHITE);    //Clean the screen with black or white.
    delay(1000);            //Delay for 1s.
    
    //8*16 font size, auto new line, black character on white back ground.
    LCD.FontModeConf(Font_8x16_1, FM_ANL_AAA, BLACK_BAC); 

    //Set the start coordinate.
    LCD.CharGotoXY(0,16);

    //Print string on I2C_LCD.
    LCD.print("Temp: ");
    //Print float on I2C_LCD.
    LCD.print(25.7, 1);
    LCD.println("`C");   
    
    LCD.print("Seconds:");
    while(1)
    {
        //Set the start coordinate.
        LCD.CharGotoXY(68,32);
        //Print the number of seconds since reset:
        // LCD.print(millis()/1000,DEC);
        LCD.print(millis(),DEC);
        delay(refreshTime);  
    }

// ----------------------------------------------

    // while(1); //Wait for ever. 
    // delay(2000);
}
