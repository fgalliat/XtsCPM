1 REM -- Main Menu --
10 CLS$=chr$(26)
20 BEL$=chr$(7)
30 CON$=chr$(127)
40 ESC$=chr$(27)
50 ABD$=ESC$+"B1"
60 ANL$=ESC$+"C1"
100 ? cls$
110 ? ""
120 ? "   "+ABD$+"-- Main Menu --"+ANL$
123 ? "" : ? ""
130 ? ABD$+"a"+ANL$+"   Beep"
140 ? ABD$+"b"+ANL$+"   Console Mode"
150 ? ABD$+"c"+ANL$+"   Cls"
198 ? ""
200 ? ABD$+"x"+ANL$+"   Exit"
250 REM INPUT c$
255 c$=INKEY$ : IF LEN(c$) = 0 THEN 250
260 IF c$ = "a" THEN ? BEL$ : GOTO 100
270 IF c$ = "b" THEN ? CON$ : GOTO 100
280 IF c$ = "c" THEN ? CLS$ : GOTO 250
290 IF c$ = "x" THEN GOTO 999
400 GOTO 250
999 STOP
