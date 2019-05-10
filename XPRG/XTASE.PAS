program testPtr;

type aStr = string[255];

{* Xtase specific BdosCall : [225..229] *}
{* procedure Bdos(reg: integer; val: word);
begin
end; *}


procedure drawBmp(text : aStr);
begin
 Bdos(225, addr(text) );
end;

procedure setConsoleColors(mode : integer);
begin
 Bdos(226, (0 shl 8) + mode );
end;

procedure setConsoleCols(mode : integer);
begin
 Bdos(226, (1 shl 8) + mode );
end;

procedure playMp3(track : integer);
begin
 { track := track + (128*256); loop }
 track := track + (64*256);
 Bdos(227, track );
end;

procedure stopMp3;
begin
 Bdos(227, 0 );
end;

{* proc w/o args don't have parent. *}
procedure clScr;
begin
  write( #26 );
end;

procedure bell;
begin
  write( #7 );
end;

procedure toggleConsoleSize;
begin
  write( #127 );
end;

procedure txtColor;
begin
  write( #27 );
  write( 'B1' );
end;

procedure txtNormal;
begin
  write( #27 );
  write( 'C1' );
end;

{* ex. play('AC#D'), play('MONKEY.T5K'), play('MARIO.T53') *}
procedure play(text : aStr);
begin
  write( #27 );
  write( '$' );
  write( text );
  write( '!' );
end;

procedure cursor(x : integer; y : integer);
begin
  write( #27 );
  write( '[' );
  write( (y+1) );
  write( ';' );
  write( (x+1) );
  write( 'H' );
end;

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
