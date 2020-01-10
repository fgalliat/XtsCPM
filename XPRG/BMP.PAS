program testBmp;

type aStr = string[255];

{* Xtase specific BdosCall : [225..229] *}
{* procedure Bdos(reg: integer; val: word);
begin
end; *}


procedure drawBmp(text : aStr);
begin
 Bdos(225, addr(text) );
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
 clScr;
 drawBmp('girl.bmp');
 writeln( 'Maybe a colorfull screen !!!!' );
end.
