program testPtr;

{* addr maybe Word / not integer *}

var
 str : string[6];

begin
 str := 'coucou';
 
 writeln( str );
 writeln( addr(str) );
end.
