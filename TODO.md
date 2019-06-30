# TODO (current work)

- [ ] If Serial monitor not connected
  - [ ] -> Keyboard reading is exctremly slow
  - [x] test if (!Serial) @ setup() time instead of while(!Serial)
  - [x] then probe if port is useable or not for further key reading
  - [x] w/ current code 15/06/19 18:04 if Yatl is wired on an external Battery/PWR

- [ ] ===================================

  - [ ] Esp
    - [x] Sym. + Space -> send 0xFF to bridge
    - [ ] ReUpload code to esp
  - [ ] Teensy 3.6
    - [x] when receives 0xFF while keyboard reading
          displayCharMap & consume 0xFF
    - [ ] ReUpload code to Teensy 3.6 




" a+ z- e* r/ t\\ y< u> i_ o= p@ "
" q$ s& d# f{ g} h[ j] k( l) m?"
" w\" x' c; v, b. n:"

- [ ] ===================================
  

- [x] upload & test MainMCU code (failure on playT5K files **FIXED** )

- [x] WiFi copy via telnet

- [x] manage MP3 module (add missing commands in XTASE.PAS demo)

- [x] make an include for XTSDEMO.PAS demo (split BdosCalls & VT-Ext routines from code)

- [ ] make **folder REFACTO** (SubMCU, RunCPM, ideas...)

- [ ] manage Keyboard Ctrl Key (cf TE.COM & TURBO.COM inner editor)

  - [ ] to be tested

- [x] manage Keyboard Left & Right Arrows

  - [ ] Left is 0x55 -> 85 (from Chatpad control values) -> choose rather Java/Javascript KeyCodes

  - [ ] Right is 0x51 -> 81

  - [x] | Key **Code** | Key             |
    | :----------- | :-------------- |
    | 37 (0x25)    | left **arrow**  |
    | 38 (0x26)    | up **arrow**    |
    | 39 (0x27)    | right **arrow** |
    | 40 (0x28)    | down **arrow**  |

- [ ] **manage key alt symbols :** ": ; . , = ? ( ) - + * / \\ $ # & { } [ ] | _ ^ @ < > "

  - [ ] 26 chars + Sym. Space -> that draws the map to screen
  - [ ] **A**(+) **Z**(-) **E**(*) **R**(/) **T**(\\) **Y**(<) **U**(>) **I**(_) **O**(=) **P**(@)
    **Q**($) **S**(&) **D**(#) **F**( { ) **G**( } ) **H**( [ ) **J**( ] ) **K**( **(** ) **L**( **)** ) **M**(?)
    **W**(") **X**(') **C**(;) **V**(,) **B**(.) **N**(:)
  - [ ] coded - to be tested
  - [ ] Sym. + SPACE injectSpeChar in keybuffer (0xFF) to tell upperLayout to
    display charMap @ screen

- [ ] **allow fillMode for drawSprites (@least INV. mode)**

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