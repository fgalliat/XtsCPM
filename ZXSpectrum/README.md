# Xts RPI0W - Arduino- ZX SPECTRUM +

02 Jan 2020

Xtase - fgalliat



**ZX Spectrum+ unit**

![ZX Spectrum+ unit](zxspec.jpg)



**Arduino Keyboard decoder**

![ZX Spectrum+ unit](zxs_arduinokb.jpg)



- [x] wire the keyboard matrix (Arduino MICRO)
  - [x] test arduino MICRO code
  - [x] remap sereval keyCodes ...
  - [x] manage debouce & repeat again
  - [ ] wire @least 2 leds for keyb state (CapLock & Ctrl)
- [ ] RPI deactivate Serial debugging (**later** : will use T3.2 SubMCU via USBSerial)
- [ ] mount a subMCU (T3.2 connected by USB)
  - [ ] mount a DFPlayer on subMCU
  
  - [ ] mount a Buzzer on SubMCU
    
    - [ ] reserve 5KB RAM for tunefiles
    
  - [ ] mount a 10LED BarGraph (could address 8 by T3.2 + 2 by MICRO (keyb) )
    - [ ] BOOT
    - [ ] DRIVE
    - [ ] WIFI
    - [ ] CAP LOCK
    - [ ] Ctrl key
    
  - [ ] ? mount an I2C 128x32 monochrome screen ?
  
  - [ ] ```
    == SubMCU Protocol ==
    
    lxy		LED x{0..7} y{0..1}			SWITCH ON/OFF 1 LED
    Lx		LEDs x{0..255}				SWITCH MASK   8 LEDs
    
    b		BEEP						EMIT a single beep
    Bxxyy	BEEP x{freqOrNote}{durMS}	TONE a sound (0..48 : notes)
    q{str}	PLAY STRING "A#CCD"			PLAY a notes sequence
    t{bin}  PLAY TUNE T5K				PLAYS a T5K tune file
    T{bin}  PLAY TUNE T53				PLAYS a T53 tune file
    
    p		(UN)PAUSE MP3				PAUSE MP3 PLAYBACK
    Pxx		PLAY MP3 x{1..999}			PLAY MP3 TRACK
    Lxx		LOOP MP3
    S		STOP MP3					STOP MP3 PLAYBACK
    vx		VOLUME x{0..30}				SET MP3 VOLUME
    M		IS PLAYING ? => {0..1}		ASK IF MP3 BUSY
    N		Next						PLAY NEXT TRACK
    V		preVious					PLAY PREV. TRACK
    
    ```
  
    
- [ ] mount USB HUB
- [ ] ? mount card reader ?
- [ ] wire HDMI output
- [ ] wire RCA output
- [ ] wire USB ext.

![ZXts unit](./zxts_cube.jpg)