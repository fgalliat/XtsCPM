program Game1;

{ Xtase YATL AdventureGame @Jul 2019 }

{$I GAME_P.INC the game places}

const
 nbTruc = 4;

var
{ place : array[1..nbPlace] of string[32];
 placeVisit : array[1..nbPlace] of integer;
}
 choice : string[32];
{ i : integer; }
 tmp: string[16];
 zz,err : integer;
begin

  initPlaces;

  zz := 0;  
  choice := '*';

  repeat

    for i := 1 to nbPlace do
    begin

      str(i, tmp);
      write( tmp +' '+ place[i] );
      str(placeVisit[i], tmp);
      write( ' You visited '+tmp+' times' );
      writeln;

    end;

    writeln( 'Your choice ?' );
    { readln(choice); }
    if ( zz = 0 ) then
      choice := '2';
    if ( zz > 0 ) then
      choice := 'q';
      

    if ( choice = 'q' ) then exit;
    if ( choice[1] in ['1'..'4'] ) then
    begin
      val( choice, i, err );
      placeVisit[i] := placeVisit[i] + 1;
    end;

    zz := zz+1;


  until ( choice = 'q' );




end.
