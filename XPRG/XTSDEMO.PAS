program testPtr;

{ Xtase YATL demo @May 2019 }

{$I XTSYATL.INC device control routines}

var
 str : string[64];
 choice : integer;

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
 cursor( 10, 5 );
 txtColor;
 writeln( 'Maybe a colorfull screen !!!!' );
 txtNormal;
 cursor( 10, 7 );
 writeln( str );

 { 60-duHast / 65-C64MENU }
 playMp3(60);

 cursor( 10, 9 );
 writeln( 'Hit Enter ....' );
 readln( choice );

 setConsoleCols(0); {* 80 cols *}
 clScr;
 writeln('Bye.');
 stopMp3;
end.
