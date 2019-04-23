1 REM Serial receive text file
2 REM Use : a:mbasic C:RTEXT
5 CLS$=chr$(26)
10 ? CLS$
15 INPUT "File name : ",FILE$
20 OPEN "O", #1, FILE$
30 INPUT ">",L$
40 IF L$ = "-EOF-" THEN GOTO 888
50 ? #1, L$
60 GOTO 30
888 CLOSE #1
890 ? "OK Written ",FILE$
895 ? "Type SYSTEM to exit..."
900 REM SYSTEM ' doesn't works