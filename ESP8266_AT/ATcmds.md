# ESP8266 AT commands

Xtase - fgalliat @Dec 2019

from : https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/

from : https://github.com/espressif/ESP8266_AT/wiki/CWLAP



- ouputs after commands may vary from ESP01 to ESP12



- waits for <CR><LF> @end of commands (on putty : use ENTER **+** CTRL-J)
- test module

  - < AT
- \>  OK
- reset module

  - < AT+RST
  - \>  OK

- local echo
  - < ATE0
  - < ATE1

- Wifi Mode

  - query
    - < AT+CWMODE?
    - \> +CWMODE:`mode` <cr> OK
  - set
    - < AT+CWMODE=`mode`
    - \> OK
  - mode is 
    - 1 STA mode
    - 2 AP mode
    - 3 both modes
    - (ESP12 has an auto host mode - see -query- to find it)

- connect to AP (wifi client)

  - query
    - < AT+CWJAP?
    - \> \+ CWJAP:`ssid` OK
  - set
    - < AT+CWJAP=`ssid`,`pwd`
    - \> OK
      - AT+CWJAP="Livebox-52B8","..."
      - AT+CWJAP="Fremen2","..."
      - have to reconnect w/ psk each time you change network
      - but keep after reset
  - list available APs (AT+CWMODE=3 before ?)
    - < AT+CWLAP
    - (more infos on the URL above)
  - disconnect from AP
    - < AT+CWQAP
    - \> OK

- STA mode (wifi ~server)

  - get infos
    - < AT+CWSAP?
  - set
    - < AT+CWSAP=`ssid`,`pwd`,`ch`,`ecn`
      - ch : chanel can be 5, 11?, ....
      - ecn : 0 open / 2 WPA_PSK / 3 WPA2_PSK / 4 WPA_WPA2_PSK
      - ex. AT+CWSAP="esp_123","1234test",5,3
  - get clients connecteds
    - < AT+CWLIF

- DHCP

  - (see infos on URL above)

- IP address

  - STA
    - < AT+CIPSTA?
    - < AT+CIPSTA=`ip`
  - AP
    - < AT+CIPAP?
    - < AT+CIPAP=`ip`

- Connections infos

  - see AT+CIPSTATUS

- TCP Client ( `id` supports MULTIPLE (server mode up to 4 clients?) conn mode, see URL...)

  - start

    - AT+CIPSTART=(`id`,)`type`,`addr`,`port`
      - type : "TCP",  "UDP"

  - send datas

    - AT+CIPSEND=(`id`,)`length`
    - AT+CIPSEND
      - Wrap return “**>**” after execute command. Enters unvarnished transmission, 20ms interval between each packet, maximum 2048 bytes per packet. When single packet containing “+++” is received, it returns to command mode.

  - close conn

    - AT+CIPCLOSE(`id`)

  - get (as client) local IP

    - AT+CIFSR

  - enable multiple client mode

    - AT+CIPMUX?
    - AT+CIPMUX=`mode`

  - Server mode

    - AT+CIPSERVER=`mode`[,`port`]

    - mode 0 : delete / 1 create

    - ##### NOTE:

      1. Server can only be created when AT+CIPMUX=1
      2. Server monitor will automatically be created when Server is created.
      3. When a client is connected to the server, it will take up one connection，be gave an id.

  - Transfert mode

    - AT+CIPMODE?
    - AT+CIPMODE=`mode` 
      - 0 : normal / 1 : unvarnished transmission mode

  - Server timeout

    - AT+CIPSTO?
    - AT+CIPSTO=<timeInSec>

  - receive datas (nothing to send just listen TX)

    - +IPD(,`id`),`len`:`data`

- test a wget like behavior

- AT+RS
  AT+CIPMODE=0
  AT+CIPMUX=1
  AT+CWJAP="MySSID"," Logon"
  AT+CIPSTART=0,"TCP","[www.myhomepage.com](http://disq.us/url?url=http%3A%2F%2Fwww.myhomepage.com%3Ak7vDq0U1lH6X4y3s8Wjzw-Wiw-w&cuid=3513344)",80
  AT+CIPSEND=0,39
  GET /info.php?a=666&b=666&c=666&d=666

- should return some +IPD ....