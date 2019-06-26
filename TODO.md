# TODO (current work)

- [ ] If Serial monitor not connected
- [ ] -> Keyboard reading is exctremly slow
- [ ] test if (!Serial) @ setup() time instead of while(!Serial)
- [ ] then probe if port is useable or not for further key reading
- [ ] w/ current code 15/06/19 18:04 if Yatl is wired on an external Battery/PWR
- [ ] ===================================
- [ ] compile MainMCU code
- [x] upload & test MainMCU code (failure on playT5K files **FIXED** the bug was in _SD_readBinFile() )
- [x] upload XTASE.PAS demo (tested **W/O** SubMCU module)
- [x] test MainMCU + SubMCU layouts connection (had to use pins 20&21 on SubMCU for Bridge RX/TX)
- [x] manage MP3 module (add missing commands in XTASE.PAS demo)
- [x] make an include for XTSDEMO.PAS demo (split BdosCalls & VT-Ext routines from code)
- [x] manage Keyboard
- [ ] manage Keyboard Ctrl Key (cf TE.COM & TURBO.COM inner editor)
- [ ] manage Joystick
- [ ] manage LEDs
- [ ] manage MP3/Busy line
- [ ] XTSDEMO.PAS as mp3 JUKEBOX demo too (play/pause/next) **(in Progress)**
- [ ] add **native JUKE.BAD reading** to make displayable the SongName
- [ ] Make Docs about pinout
- [ ] Make Docs about VT-Ext + BdosCall extension mapping **(in Progress)**
- [x] GFX Drawing routines (@least to make a TP3 benchmark)
  - beware : as screen coords are > 255 need uint16_t
  - can PString contains 0x00 w/ trimming (Cf [0] contains length -init or current ?)
  - have to bench on Testing BdosCall
  - else will have to use ASCII coords + atoi() but it will be slower
  - [drawShape][shapeType][fillMode][color][x][y][w][h]
     0x7F        0x01       0x00     65535  310(16), 230(16), 10(16),10(16)
     to draw a WHITE Rect lower-right
     could still use Pseudo palette color if color < 16 => \<have TODO that 'cause can't use Word w/ TP3>
  - 100x100 filled rect is immediate !!! 5x is still immediate !!! 30x is ~ immediate !!
  - ReMapped drawRoutines on BdosCall(225) ...
  - ADDED Sprites Support (40KB free for Sprites - 160x120 board)