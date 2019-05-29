# More Ideas

- **Esp12 as SubMCU**
  - needs :
    - I2C for SX1509 (Keyb) **(GPIO 4&5 D1&D2)**
    - Serial for bridge **(GPIO 1&3)**
    - SoftwareSerial for DFPlayer (MP3) **(GPIO 12&14 ?)** - <https://github.com/plerup/espsoftwareserial>
    - WiFi
      - client (AP/STA mode) - wget style
      - telnet server - @least as inputStream / bridge2teensySd
        - <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino>
  - can't :
    - use **GPIO 0,2,15** cf flashing/running mode
  - <https://alselectro.wordpress.com/2016/11/07/esp8266-upload-code-from-arduino-ide-no-arduino-board-required/>
- programming
    - <https://www.fais-le-toi-meme.fr/fr/electronique/tutoriel/programmes-arduino-executes-sur-esp8266-arduino-ide>
  
- **328P as screen pad controller**
  - needs :
    - Serial for bridge (/!\\ if 5v : need a voltage divider on TX)
    - GPIOs for BTNs + cross
    - GPIOs for LEDs ?