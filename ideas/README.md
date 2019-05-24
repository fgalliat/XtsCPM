# More Ideas

- **Esp12 as SubMCU**
  - needs :
    - I2C for SX1509 (Keyb) **(GPIO 4&5)**
    - Serial for bridge **(GPIO 1&3)**
    - SoftwareSerial for DFPlayer (MP3) **(GPIO 12&14 ?)** - <https://github.com/plerup/espsoftwareserial>
    - WiFi
      - client (AP/STA mode) - wget style
      - telnet server - @least as inputStream / bridge2teensySd
        - <https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino>
  - can't :
    - use **GPIO 0,2,15** cf flashing/running mode

- **328P as screen pad controller**
  - needs :
    - Serial for bridge (/!\\ if 5v : need a voltage divider on TX)
    - GPIOs for BTNs + cross
    - GPIOs for LEDs ?