
// uint16_t read16(fs::File &f);
// uint32_t read32(fs::File &f);
uint16_t read16(File &f);
uint32_t read32(File &f);

static void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  File bmpFS;

  drive_led(true);
  // Open requested file on SD card
  bmpFS = SD.open(filename);

  if (!bmpFS)
  {
    Serial.print("File not found");
    drive_led(false);
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      // TFT_eSPI specific may cause WEIRD_MODE on drawPAK !!!!!
      // tft.setSwapBytes(true);

      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }
        // Read any line padding
        if (padding) bmpFS.read((uint8_t*)tptr, padding);
        // Push the pixel row to screen, pushImage will crop the line if needed
        // tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        tft.drawRGBBitmap(x, y--, (uint16_t*)lineBuffer, w, 1);
      }
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
  drive_led(false);
}


uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}


uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@ Sprites
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



#if SPRITES_SUPPORT

   uint16_t spriteArea[ SPRITE_AREA_SIZE ];
   Sprite sprites[NB_SPRITES];

   int spriteInstanceCounter = 0; 
   int lastAddr = 0;

   void Sprite::drawClip(int x, int y) {
      if ( w < 0 || h < 0 ) { return; }
      if ( !isValid() ) { return; }

      uint16_t row[ w ];
      for(int i=0; i < h; i++) {
         if ( i+y >= TFT_HEIGHT ) { break; }
         // *2 cf uint16_t
         memcpy( &row[0], &spriteArea[ ( (this->y+i) * SPRITE_AREA_WIDTH )+this->x ], w*2 );
         // tft.writeRect(x, i+y, w, 1, row);
         tft.drawRGBBitmap(x, i+y, row, w, 1);
      }

      // tft.writeRect(50, 50, 160, 120, spriteArea);
   }
   
    void yat4l_tft_cleanSprites() {
       memset(spriteArea, 0, SPRITE_AREA_SIZE);
       spriteInstanceCounter = 0; 
       lastAddr = 0;
       for(int i=0; i < NB_SPRITES; i++) {
          sprites[i].invalid();
       }
    }

    void _feedSprites(char* filename, int x, int y);


    void yat4l_tft_grabbSprites(char* imageName, int offsetX, int offsetY) {
       char* fileName = yat4l_fs_getAssetsFileEntry( imageName );
       _feedSprites(fileName, offsetX, offsetY);
    }

    void yat4l_tft_grabbSpritesOfSize(char* imageName, int offsetX, int offsetY, int width, int height) {

      //  char* fileName = this->yatl->getFS()->getAssetsFileEntry( imageName );
      char* fileName = yat4l_fs_getAssetsFileEntry( imageName );

       yat4l_tft_cleanSprites();
       int nbW = 160/width;
       int nbH = 120/height;
       int howMany = nbW * nbH;
       if ( howMany > NB_SPRITES ) { howMany = NB_SPRITES; }
       int cpt = 0;
       for(int y=0; y < nbH; y++) {
         for(int x=0; x < nbW; x++) {
            sprites[cpt].x = x*width;
            sprites[cpt].y = y*height;
            sprites[cpt].w = width;
            sprites[cpt].h = height;
            cpt++;
            if ( cpt >= howMany ) { break; }
         }
       }
       _feedSprites(fileName, offsetX, offsetY);
    }

    #define BUFFPIXEL 80

   // will takes only 160x120 px of bmp file
   void _feedSprites(char* filename, int x, int y) {
      if ( filename == NULL || strlen(filename) <= 0 || strlen(filename) >= 32 ) {
         yat4l_dbug("(WW) Wrong BMP filename !");
         return;
      }
 
      File bmpFile;
      int bmpWidth, bmpHeight;             // W+H in pixels
      uint8_t bmpDepth;                    // Bit depth (currently must be 24)
      uint32_t bmpImageoffset;             // Start of image data in file
      uint32_t rowSize;                    // Not always = bmpWidth; may have padding
      uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
      uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
      boolean goodBmp = false;             // Set to true on valid header parse
      boolean flip = true;                 // BMP is stored bottom-to-top
      int w, h, row, col;
      uint8_t r, g, b;
      uint32_t pos = 0;//, startTime = millis();

      uint16_t awColors[320]; // hold colors for one row at a time...

      // if ((x >= tft.width()) || (y >= tft.height()))
      //    return;

      // Open requested file on SD card
      if (!(bmpFile = SD.open(filename)))
      {
         yat4l_warn("BMP File not found");
         return;
      }

      // Parse BMP header
      if (read16(bmpFile) == 0x4D42) { // BMP signature
         (void)read32(bmpFile);

         (void)read32(bmpFile);            // Read & ignore creator bytes
         bmpImageoffset = read32(bmpFile); // Start of image data
         (void)read32(bmpFile);

         bmpWidth = read32(bmpFile);
         bmpHeight = read32(bmpFile);
         if (read16(bmpFile) == 1)     { // # planes -- must be '1'
            bmpDepth = read16(bmpFile); // bits per pixel
            if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
            goodBmp = true; // Supported BMP format -- proceed!

            // BMP rows are padded (if needed) to 4-byte boundary
            rowSize = (bmpWidth * 3 + 3) & ~3;

            // If bmpHeight is negative, image is in top-down order.
            // This is not canon but has been observed in the wild.
            if (bmpHeight < 0) {
               bmpHeight = -bmpHeight;
               flip = false;
            }

            if ((x >= bmpWidth) || (y >= bmpHeight)) {
               yat4l_warn("Sprite OutOfBounds");
               return;
            }

            // Crop area to be loaded
            // w = bmpWidth;
            // h = bmpHeight;
            w = SPRITE_AREA_WIDTH; h = SPRITE_AREA_HEIGHT;
            if ((x + w - 1) >= bmpWidth)
               w = bmpWidth - x;
            if ((y + h - 1) >= bmpHeight)
               h = bmpHeight - y;

            for (row = 0; row < h; row++) { // For each scanline...

               // Seek to start of scan line.  It might seem labor-
               // intensive to be doing this on every line, but this
               // method covers a lot of gritty details like cropping
               // and scanline padding.  Also, the seek only takes
               // place if the file position actually needs to change
               // (avoids a lot of cluster math in SD library).
               if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                  pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
               else // Bitmap is stored top-to-bottom
                  pos = bmpImageoffset + row * rowSize;
               if (bmpFile.position() != pos) { // Need seek?
                  bmpFile.seek(pos);
                  buffidx = sizeof(sdbuffer); // Force buffer reload
               }

               for (col = 0; col < w; col++) { // For each pixel...
                  // Time to read more pixel data?
                  if (buffidx >= sizeof(sdbuffer)) { // Indeed
                     bmpFile.read(sdbuffer, sizeof(sdbuffer));
                     buffidx = 0; // Set index to beginning
                  }

                  // Convert pixel from BMP to TFT format, push to display
                  b = sdbuffer[buffidx++];
                  g = sdbuffer[buffidx++];
                  r = sdbuffer[buffidx++];
                  awColors[col] = tft.color565(r, g, b);
               } // end pixel

               // tft.writeRect(0, row, w, 1, awColors);
               // *2 Cf uint16_t
               memcpy( &spriteArea[ (row*SPRITE_AREA_WIDTH)+col ], &awColors[x], w*2 );

            } // end scanline
            // long timeElapsed = millis() - startTime;
            } // end goodBmp
         }
      }

      bmpFile.close();
      if (!goodBmp) {
         yat4l_warn("BMP format not recognized.");
      }

   }
   #endif