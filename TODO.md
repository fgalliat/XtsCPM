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

    - then try bigger memseg that screen caps
      - ex. 80x25 on 53x25
      - just for compatibility of softwares display

    - try to find the tinyFont for that lib **(DONE)**

      

- Console Mode Switcher **(DONE)**

    - 40 to 80 cols
    - (80 cols current font doesn't have lowerCase chars)
    - make a BDOS interrupt entry ... (maybe later)
    - or make a specific esc char sequence ( **0x7F** 127 (seems that console is 7bits per default) )

  

- VTxx emulation

  - http://matthieu.benoit.free.fr/68hc11/vt100.htm
  - https://espterm.github.io/docs/vt100.codes.txt
  - not really that ...
  - <ESC> B1  =====> try : will begin color
  - <ESC> C1  =====> try : will end color****
  - **WORKS :** 
    - **TURBO** (Pascal) works (even if could be better)
    - **OS** works
  - **DOESN'T WORKS:**
    - **T**ext**E**ditor doesn't works very well
      - cause not 80chars
      - cause some different escapes
