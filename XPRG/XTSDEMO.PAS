program testPtr;

{ Xtase YATL demo @May 2019 }

{$I XTSYATL.INC device control routines}

var
 str : string[64];
 choice : string[1];
 trk: integer;

begin
 str := 'Let s play an MP3 now !';

 setConsoleColors(1); {* LCD mono *}
 setConsoleCols(1); {* 40 cols *}
 clScr; {* no args > no parent. *}
 cursor( 10, 5 );
 writeln( 'How, I m a poor lcd mono display' );
 cursor( 10, 7 );
 writeln( 'Let s play with Buzzer' );
 cursor( 0, 9 );
 play('MONKEY.T5K'); {* wait keyPress *}

 setConsoleColors(0); {* phosphore mode *}
 clScr;
 drawBmp('girl.bmp');
 
 drawRect(10,10,100,100);

 cursor( 10, 5 );
 txtColor;
 writeln( 'Maybe a colorfull screen !!!!' );
 txtNormal;
 cursor( 10, 7 );
 writeln( str );

 { 60-duHast / 65-C64MENU }
 playMp3(60);

 cursor( 10, 9 );
 writeln( 'Choose Action (m,s,p,n,c,q) ...' );
 repeat 
   readln( choice );
   if ( choice = 'm' ) then 
    playMp3(65);
   if ( choice = 's' ) then
    stopMp3;
   if ( choice = 'p' ) then
    pauseMp3;
   if ( choice = 'n' ) then
    nextMp3;
   if ( choice = 'c' ) then
    begin
     writeln('TrackNum ?');
     readln(trk);
     playMp3(trk);
    end;
 until ( choice = 'q') or (choice = '' );

 setConsoleCols(0); {* 80 cols *}
 clScr;
 writeln('Bye.');
 stopMp3;
end.
