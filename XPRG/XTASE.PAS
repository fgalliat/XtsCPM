program testPtr;

type aStr = string[255];


procedure testBdosHook(text : aStr);
begin
 {* Xtase specific BdosCall *}
 {* addr maybe Word / not integer *}
 Bdos(225, addr(text) );
end;

procedure drawBmp(text : aStr);
begin
 {* Xtase specific BdosCall *}
 {* addr maybe Word / not integer *}
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


var
 str : string[64];

begin
 str := 'coucou les gens !';

 clScr; {* no args > no parent. *}

 drawBmp('girl.bmp');

 txtColor;
 writeln( str );
 txtNormal;
 bell;

 { testBdosHook('Salut les cons !'); }

 { play('MARIO.T53'); }
 play('MONKEY.T5K');
end.
