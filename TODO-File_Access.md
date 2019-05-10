# TODO - File Access

- **File support**

  - try **XMODEM** command to send source code to YATL CP/M

  - @ least while TE doesn't display well

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

    - for small text files (such as source codes)
      - did : a:mbasic C:RTEXT
      - to write a single file on current disk
        - echo "a:mbasic C:RTEXT" > /dev/ttyACM0
        - echo "TOTO.PAS" > /dev/ttyACM0
        - cat ./TOTO.PAS > /dev/ttyACM0
        - echo "SYSTEM" > /dev/ttyACM0



- https://archive.org/stream/BASIC-80_MBASIC_Reference_Manual/BASIC-80_MBASIC_Reference_Manual_djvu.txt

