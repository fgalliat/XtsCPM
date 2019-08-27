# ESP32 TFT_eSPI for ILI9341

- in User_Setup.h

\#define TFT_MISO 19

\#define TFT_MOSI 23

\#define TFT_SCLK 18

\#define TFT_CS    5  // Chip select control pin

\#define TFT_DC   15  // Data Command control pin

//#define TFT_RST   4  // Reset pin (could connect to RST pin)

\#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST