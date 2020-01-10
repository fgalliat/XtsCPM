# WaveShare 4inch RPI display

<https://www.waveshare.com/4inch-rpi-lcd-a.htm>



320*480 (Pixel) => 480x320 => 



- Vs 2.8" :: beware w/ POWER consumption ....



## Interface

|            PIN NO.             |      SYMBOL      |                         DESCRIPTION                          |
| :----------------------------: | :--------------: | :----------------------------------------------------------: |
|             1, 17              |       3.3V       |              Power positive (3.3V power input)               |
|              2, 4              |        5V        |               Power positive (5V power input)                |
| 3, 5, 7, 8, 10, 12, 13, 15, 16 |        NC        |                              NC                              |
|        6, 9, 14, 20, 25        |       GND        |                            Ground                            |
|               11               |      TP_IRQ      | Touch Panel interrupt, low level while the Touch Panel detects touching |
|               18               |      LCD_RS      |             Instruction/Data Register selection              |
|               19               |  LCD_SI / TP_SI  |              SPI data input of LCD/Touch Panel               |
|               21               |      TP_SO       |                SPI data output of Touch Panel                |
|               22               |       RST        |                            Reset                             |
|               23               | LCD_SCK / TP_SCK |                 SPI clock of LCD/Touch Panel                 |
|               24               |      LCD_CS      |                LCD chip selection, low active                |
|               26               |      TP_CS       |            Touch Panel chip selection, low active            |



<https://github.com/Bodmer/TFT_eSPI>

in TFT_eSPI : User_Select.h

//#define RPI_ILI9486_DRIVER // 20MHz maximum SPI

- **looks @ bodmer comments**

- <https://forum.arduino.cc/index.php?topic=499672.0>

  // Customised User_Setup files are stored in the "User_Setups" folder.

// Only ONE line should be uncommented.  Add extra lines and files as needed.

//#include <User_Setup.h>           // Default setup is root library folder

//#include <User_Setups/Setup1_ILI9341.h>  // Setup file configured for my ILI9341
//#include <User_Setups/Setup2_ST7735.h>   // Setup file configured for my ST7735
//#include <User_Setups/Setup3_ILI9163.h>  // Setup file configured for my ILI9163
//#include <User_Setups/Setup4_S6D02A1.h>  // Setup file configured for my S6D02A1
//#include <User_Setups/Setup5_RPi_ILI9486.h>     // Setup file configured for my stock RPi TFT
//#include <User_Setups/Setup6_RPi_Wr_ILI9486.h>  // Setup file configured for my modified RPi TFT
//#include <User_Setups/Setup7_ST7735_128x128.h>  // Setup file configured for my ST7735 128x128 display
//#include <User_Setups/Setup8_ILI9163_128x128.h>  // Setup file configured for my ILI9163 128x128 display
//#include <User_Setups/Setup9_ST7735_Overlap.h>   // Setup file configured for my ST7735
//#include <User_Setups/Setup10_RPi_touch_ILI9486.h>  // Setup file configured for my stock RPi TFT with touch
\#include <User_Setups/Setup11_RPi_touch_ILI9486.h>  // Setup file configured for my stock RPi TFT with touch

//#include <User_Setups/SetupX_Template.h> // Setup file template for copying/editting  

Display SDO/MISO  to NodeMCU pin D6
Display LED       to NodeMCU pin VIN (or 5V)
Display SCK       to NodeMCU pin D5
Display SDI/MOSI  to NodeMCU pin D7
Display DC (or AO)to NodeMCU pin D3
Display RESET     to NodeMCU pin D4
Display CS        to NodeMCU pin D8
Display GND       to NodeMCU pin GND (0V)
Display VCC       to NodeMCU 5V or 3.3V
Display Touch_CD to NodeMCU pin D1











- mod for faster display on ILI9486 : 
- <https://forum.arduino.cc/index.php?topic=443787.msg3194490#msg3194490>



(<https://forum.arduino.cc/index.php?topic=548380.0>)

  Hi,

I have purchased a 4.0 inch WaveShare TFT screen ( im trying out loads to see which works best ).

The driver is a bit poor, and i was wondering what other people have used?

The spec is here

https://www.waveshare.com/4inch-TFT-Touch-Shield.htm

Key Parameters
LCD Type   TFT
LCD Interface   SPI
LCD Controller   ILI9486
Touch Screen Type   Resistive
Touch Screen Controller   XPT2046
Colors   RGB, 65K colors
Resolution   480x320 (Pixel)
Aspect Ratio   8:5
I/O Voltage   3.3V/5V

Interface

Symbol   Arduino / NUCLEO PIN   Description
5V   5V   5V power input
GND   GND   Ground
SCLK   D13   SPI clock
MISO   D12   SPI data input
MOSI   D11   SPI data output
LCD_CS   D10   LCD chip select
LCD_BL   D9   LCD backlight
LCD_RST   D8   LCD reset
LCD_DC   D7   LCD data/command selection
TP_BUSY   D6   Touch panel busy
SD_CS   D5   Micro SD card chip select
TP_CS   D4   Touch panel chip select
TP_IRQ   D3   Touch panel interrupt  