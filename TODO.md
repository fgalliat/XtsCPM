## TODOs


 - TTY emulation on TFT Screen
   - tinyFont 3x5 => 4x6 => @320x240 => 80x40 chars

   - regular font 6x8 => 53x30 chars

   - ILI9341_t3n scrolling impl. is very slow for now
     - when no scrolling & screen overflow : seems to deadlock the teensy
     - have to make a console in RAM (80x30 gives 3200 bytes) **[in progress & working]**
     - use that cls/write abstraction to provide VT100 escape chars emulation ?
        - ^C // \b // \t // \r \n
        - cls
        - color
        - locate ? / bold ? (could be just diff color)
          - if text has attribs I'll have to store those
          - by spans / by chars :-( => on WSL & ubuntu that chars attrs seems to be ignored ....
          - as memseg is 1590 or 3200 -> I choosed to double it & make an attribMap
            - not the better idea but have enought RAM & faster solution

    - 1st, try a benchmark : 
      - allocate memseg (1590 bytes for 53x30) **[v]**
      - fill all w/ a character **[v]**
      - display whole mem-console **[v]**
      - try to scroll **[v]**
      - see rendering time **[nearly good]** 

    - then try bigger memseg that screen caps **(DONE)**
      - ex. 80x25 on 53x25
      - just for compatibility of softwares display

    - try to find the tinyFont for that lib **(DONE)**

      

- Console Mode Switcher **(DONE)**

    - 40 to 80 cols
    - (80 cols current font doesn't have lowerCase chars)
    - make a BDOS interrupt entry ... (maybe later)
    - or make a specific esc char sequence ( **0x7F** 127 (seems that console is 7bits per default) )
      - can use \$> ? chr\$(127) with mBASIC

  

- VTxx emulation

  - http://matthieu.benoit.free.fr/68hc11/vt100.htm
  - https://espterm.github.io/docs/vt100.codes.txt
  - not really as simple as that ...
  - <ESC> B1  =====> try : will begin color
  - <ESC> C1  =====> try : will end color****
  - <Esc> 26 is CLS
  - **WORKS :** 
    - **TURBO** (Pascal) works (even if could be better)
    - **OS** works
  - **DOESN'T WORKS:**
    - **T**ext**E**ditor doesn't works very well
      - cause some different escapes
  - **in Progress**



- **File support**
  - try **XMODEM** command to send source code to YATL CP/M

  - @ least while TE doesn't display well

  - try to make a Pascal prgm to handle console mode switch (? chr$(127) like)

  - XMODEM toto.pas /R to receive
    - **start** minicom on PC > Send file by XMODEM
      - on Nux Box can try "lrzsz" package
    - **then** A:XMODEM toto.pas /R
    - but need to input by Serial **+** ChatPad
    - so unusable for now ...

  - XMODEM toto.pas /S to send

  - does really **XMODEM** wait on **Arduino Serial** or does it use a spe device descriptor ?

  - **XMODEM.COM** seems to require **CONSOLE8.COM** to work properly

  - on Nux Box, try **sz -X TEST.BAS** to send a file

    - echo "CONSOLE8" > /dev/ttyACM0
    - echo "XMODEM C:TEST.TXT /R" > /dev/ttyACM0
      - says that it receives via RDR/PUN

  - may write a code thats reads **stdIn** directly then write to file (w/ an end sequence ex. \n-EOF- )

  - ```
    Example: 1000 'TIMED INPUT SUBROUTINE 
    1010 RESPONSE$="" 
    1020 FOR I%=1 TO TIMELIMIT% 
    1030 A$=INKEy$ : IF LEN(A$)=0 THEN 1060 
    1040 IF ASC(A$)=13 THEN TIMEOUT%=0 : RETURN 
    1050 RESPONSE$=RESPONSE$+A$ 
    1060 NEXT 1% 
    1070 TIME0UT%=1 t RETURN 
    
    
    http://www.antonis.de/qbebooks/gwbasman/chapter%205.html
    5.2.1 Creating a Sequential File
    
    The following statements and functions are used with sequential files:
    CLOSE 	LOF
    EOF 	OPEN
    INPUT# 	PRINT#
    LINE INPUT#	PRINT# USING
    LOC 	UNLOCK
    LOCK 	WRITE#
    
    The following program steps are required to create a sequential file and access the data in the file:
    
        Open the file in output (O) mode. The current program will use this file first for output:
    
        OPEN "O",#1,"filename"
    
        Write data to the file using the PRINT# or WRITE# statement:
    
        PRINT#1,A$
        PRINT#1,B$
        PRINT#1,C$
    
        To access the data in the file, you must close the file and reopen it in input (I) mode:
    
        CLOSE #1
        OPEN "I",#1,"filename
    
        Use the INPUT# or LINE INPUT# statement to read data from the sequential file into the program:
    
        INPUT#1,X$,Y$,Z$
    
    
    OPEN "O",#1,"TEST.TXT"
     INPUT L$
     PRINT #1 L$
    CLOSE #1
    
    
    ```



- https://archive.org/stream/BASIC-80_MBASIC_Reference_Manual/BASIC-80_MBASIC_Reference_Manual_djvu.txt





- **Keyboard support**

  - Microsoft XBOX 360 Chatpad

    - stdIO handling (Serial IN / Chatpad IN -- Serial OUT / TFT OUT)

    - Spe keys

      - will need to have a **CTRL like key** (ex. for **TE**)

      - greenSquare & redCircle are all used to make speChars

      - **no ESC key** // a bckSp key 

      - have a -peopleKey- that could emulate CTRL Key

      - may **-peopleKey- + [<-]** could do **ESC**

      - then **-peopleKey- + [->]** could do another spe key 

        - maybe YATL specific hdl : 

        - [MENU]

          \> TOGGLE CONSOLE MODE

          \> SELF TEST

          \> REBOOT

  

- **Buzzer support**

  

- **LED support**