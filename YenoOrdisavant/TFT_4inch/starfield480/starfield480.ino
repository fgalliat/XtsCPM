/**
 * ESP32 working w/ arduino IDE
 * 
 * Additional board url : https://dl.espressif.com/dl/package_esp32_index.json
 * goto Board Manager
 * choose ESP32 by espressif
 * 
 * DOIT ESP32 DEVKIT V1
 * 
 * 1.3 MB Sktech
 * 320 KB RAM
 * 
 * 
 * TFT_eSPI w/ ILI9486 4inch 480x320 pixels
 * 
 * don't forget the RESET LINE
 * 
 * BEWARE : TouScreenCS is D4 : as same as SD_CS for now
 * 
 */


// Animates white pixels to simulate flying through a star field

#include <SPI.h>
#include <TFT_eSPI.h>

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

// With 1024 stars the update rate is ~65 frames per second
#define NSTARS 1024
uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

uint8_t za, zb, zc, zx;

// Fast 0-255 random number generator from http://eternityforest.com/Projects/rng.php:
uint8_t __attribute__((always_inline)) rng()
{
  zx++;
  za = (za^zc^zx);
  zb = (zb+za);
  zc = (zc+(zb>>1)^za);
  return zc;
}

void setup() {
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

  // Serial.begin(115200);
  Serial.begin(9600);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // fastSetup() must be used immediately before fastPixel() to prepare screen
  // It must be called after any other graphics drawing function call if fastPixel()
  // is to be called again
  //tft.fastSetup(); // Prepare plot window range for fast pixel plotting
}

// #define MI_W 160
// #define MI_H 120
#define MI_W 240
#define MI_H 160

void loop()
{
  unsigned long t0 = micros();
  uint8_t spawnDepthVariation = 255;

  for(int i = 0; i < NSTARS; ++i)
  {
    if (sz[i] <= 1)
    {
      sx[i] = MI_W - MI_H + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
    }
    else
    {
      int old_screen_x = ((int)sx[i] - MI_W) * 256 / sz[i] + MI_W;
      int old_screen_y = ((int)sy[i] - MI_H) * 256 / sz[i] + MI_H;

      // This is a faster pixel drawing function for occassions where many single pixels must be drawn
      tft.drawPixel(old_screen_x, old_screen_y,TFT_BLACK);

      sz[i] -= 2;
      if (sz[i] > 1)
      {
        int screen_x = ((int)sx[i] - MI_W) * 256 / sz[i] + MI_W;
        int screen_y = ((int)sy[i] - MI_H) * 256 / sz[i] + MI_H;
  
        if (screen_x >= 0 && screen_y >= 0 && screen_x < (MI_W*2) && screen_y < (MI_H*2) )
        {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          tft.drawPixel(screen_x, screen_y, tft.color565(r,g,b));
        }
        else
          sz[i] = 0; // Out of screen, die.
      }
    }
  }
  unsigned long t1 = micros();
  //static char timeMicros[8] = {};

 // Calcualte frames per second
  Serial.println(1.0/((t1 - t0)/1000000.0));
}


