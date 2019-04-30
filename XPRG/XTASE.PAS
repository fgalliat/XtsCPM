program testPtr;

{* addr maybe Word / not integer *}

var
 str : string[64];

begin
 str := 'coucou les gens !';
 
 writeln( str );
 writeln( addr(str) );

 {* Xtase specific BdosCall *}
 Bdos(225, addr(str) );
end.
