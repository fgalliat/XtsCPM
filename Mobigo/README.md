## Reuse of Vtech Mobigo Gaming Console

### Sliding console with Keyboard

**Keyboard**

  - 10 cols x 5 rows matrix Keyboard w/ 35 keys
  - @ this time : 9x5 really useable for 34 keys

  - driven by an SX1509 GPIO I2C expander
    - 5rows as OUTPUT
    - 10cols as INPUT (no pullUp)

    - Behavior to read 1 key :
      - send HIGH for only 1 ROW
      
      - read each COL input
      
  - send LOW for that ROW
      
      - gives a ROW,COL address
      
        
    
  - TODO :
    - make some key maps (regular, shift, ...) **(in Progress)**
    - change layout from ABCDEF (french edition) to AZERTY or QWERTY
    - make buffer for keys **(done)**
    - handle key combos (nums, symbols, Ctrl) **(made as remanant state)**
    - find missing key : [ ? ] **(not found for now)**
      - could act as Ctrl, else have to find another combo
    - allow multiple key read on same row **(doesn't works for now)**
    - distinct pollKey Vs readKey **(done)**
    - re-use fold/open detector push  contact -> wire it on SX1509
    - not able to scan combos yet -> make as remanant ????
    - nums should be toggle instead of remanant ????
    - **Symbols for CP/M**
      - : . \n \b \s + " ? / "
      - Symb + Z -> ':' **(done)**
      - Symb + Y -> '.' **(done)**
      - now able to type c:xtsdemo.pas
    

**Teensy 3.6 Yatl pinout** 

**I2C : ** 0 -> 18 & 19 (not used by SPI TFT)

**SPI Screen :**

    // w/ that lib BOTH CS & DC needs to besome CS pins
    #define _TFT_DC  20
    #define _TFT_CS  15
    // #define TFT_SCK 14 // doesn't work !!!!
    #define TFT_SCK 13
    
    #define TFT_MOSI 11
    #define TFT_MISO 12
    #define TFT_RST  17
    #define TFT_LED  16
