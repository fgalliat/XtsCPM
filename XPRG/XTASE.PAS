program testPtr;

type aStr = string[255];


procedure testBdosHook(text : aStr);
begin
 {* Xtase specific BdosCall *}
 {* addr maybe Word / not integer *}
 Bdos(225, addr(text) );
end;


var
 str : string[64];

begin
 str := 'coucou les gens !';
 writeln( str );
 
 testBdosHook('Salut les cons !');
end.
