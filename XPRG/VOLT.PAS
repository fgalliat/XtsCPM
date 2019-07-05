program voltage;
{* Voltage reader *}

var
  ret: integer;
  vlt: real;
begin
  ret := Bdos( 228, 0 shl 8 );
  { writeln( ret ); }
  vlt := ret * 5.0 / 256.0;
  writeln( vlt );
end.