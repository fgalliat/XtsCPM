program testPtr;

type aStr = string[255];


procedure drawBmp(text : aStr);
begin
 {* Xtase specific BdosCall *}
 Bdos(225, addr(text) );
end;

procedure setConsoleColors(mode : integer);
begin
 {* Xtase specific BdosCall *}
 Bdos(226, mode );
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
 str := 'coucou les gens !';

 setConsoleColors(1); {* LCD mono *}
 clScr; {* no args > no parent. *}
 cursor( 10, 5 );
 writeln( 'How, I m a poor lcd mono display' );
 cursor( 10, 7 );
 writeln( 'Hit Enter ....' );
 bell;
 readln( choice );

 setConsoleColors(0); {* phosphore mode *}
 clScr;
 drawBmp('girl.bmp');
 cursor( 10, 5 );
 txtColor;
 writeln( 'Maybe a colorfull screen !!!!' );
 txtNormal;

 cursor( 10, 7 );
 writeln( str );

 cursor( 0, 9 );
 

 { play('MARIO.T53'); }
 play('MONKEY.T5K');
end.
