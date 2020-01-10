program GotoScreen;

procedure cursor(x : integer; y : integer);
begin
  write( #27 );
  write( '[' );
  write( (y+1) );
  write( ';' );
  write( (x+1) );
  write( 'H' );
end;


begin
  clrscr;

  GotoXY(1, 1); write('Coucou 1');
  cursor(20, 2); write('Coucou 2');

  GotoXY(1, 3); write('Coucou 3');
  cursor(20, 3); write('Coucou 4');

  GotoXY(1, 5); write('Coucou 5');
  cursor(20, 4); write('Coucou 6');

end.