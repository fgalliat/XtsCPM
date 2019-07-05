program Test;
{* Test Bdos 229 *}

var
  ret: integer;
begin
  writeln( 'Testing ...' );
  ret := Bdos( 229, 0 shl 8 );
end.