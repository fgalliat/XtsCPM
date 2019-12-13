# TODO YAT4L

- Enclosure
- LED
- Buzzer
- DFPlayer
- Keyboard
  - HT USB board ?
  - MS ChatPad ?
- Serial To VGA board ?
  - both VGA Text boards are soldered but one is on a BUS-CARD
  - or T3.2 T3.6 VGA out lib => https://github.com/qix67/uVGA
  - Teensy++2 w/ big RAM TvOut (https://github.com/Avamander/arduino-tvout)
    - AT90USB1286 MCU (support for TVOut) + 1KR on SYNC(5) / 470R on VID(7) // GND
    - have to see MAX resolution I could have
    - 160 width seems to be the max (because of pin usage)
    - https://sourceforge.net/p/arduino-tvout/wiki/Useage/
- WiFi
  - ESP12 or ESP01 ?
  - ATcmds or SubMCU code ?
  - API (AT cmds + Java API for now)
    - [x] test
    - [x] list APs / connect AP
    - [x] set STA/AP
    - [x] wget(url) (**to finish**)
      - [ ] act as a REST GET client (see Bearers too)
      - [ ] add getHomeServer() method based on current SSID
      - [ ] Z:/wifi.psk => ($HOME_SSID$:$HOME_HOST$:$REMOTE_HOST$\n) + ( $SSID$:$PSK$\n )[]
    - [ ] startTelnetD (**later**)
    - [x] getIP() (STA/AP mode same API method)
    - [x] getSSID (STA/AP)
    - [x] disconnect from AP
    - [x] reset
- TTS via 328P 16MHz 5v
- Thermal Printer (9v)

- Bdos ReadSerial / WriteSerial
   -> w/ SerialXX choose
   -> try to write a full SerialTerm App
   -> ex. to TEST ESP8266, to check bridge, or Ext. Aux. Port ...



- take a look at older CP/M system w/ Nano

https://www.eevblog.com/forum/vintage-computing/cpm-system-with-ps2-keyboard-tv-out-powered-by-arduino-nano/
