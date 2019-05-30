# Esp12 as SubMCU

**a) pinout**

- **RX,TX** bridge
- **D5, D6** (GPIO 14,12) SoftSerial DFPlayer
- **D1, D2** (GPIO A4,A5) I2C SX1509
- A0 (GPIO A0) MP3 isPlaying INPUT
- D4, D0, D7 (GPIO 2, 16, 13) LED OUTPUTs
- **RESERVED** GPIO 0,2,15 for flashing control
- **BEWARE** w/ multiple GND : not really linked



**b)commands**

- i -> infos
- k -> read keyb buffer
- w -> WiFi
  - c : connect
    - s  : STA mode ( w/ SSID[] )
    - a : AP mode
  - s : stop
  - i : getIP
  - e : getESSID
  - t : telnet
    - o : open
    - c : close
  - g : get URL content (wget/curl like)
- p -> mp3 control
  - p -> play track
  - ...