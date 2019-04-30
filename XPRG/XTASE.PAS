program testPtr;

type aStr = string[255];


procedure testBdosHook(text : aStr);
begin
 {* Xtase specific BdosCall *}
 {* addr maybe Word / not integer *}
 Bdos(225, addr(text) );
end;

procedure clScr;
begin
  write( #26 );
end;


var
 str : string[64];

begin
 str := 'coucou les gens !';

 clScr;
 writeln( str );

 testBdosHook('Salut les cons !');
end.
