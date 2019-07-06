program Game1;

{ Xtase YATL AdventureGame @Jul 2019 }

{$I XTSYATLG.INC the Yatl GFX include}
{  GAME_P.INC the game places}
{$I GAME_S.INC the game scenes}

const
 nbTruc = 4;

var
 pack : string[8];
 choice : string[32];
{ i : integer; }
 tmp: string[16];
 zz,err : integer;
 curPlace : integer;
begin

{
  initPlaces;
  pack := 'ishar';
  curPlace := 6;

  zz := 0;  
  choice := '*';

  repeat

    clrscr;
    drawPakImage(pack, curPlace, 0, 0);
    gotoXY(1,1);

    for i := 1 to nbPlace do
    begin
      str(i, tmp);
      write( tmp +' '+ place[i] );
      str(placeVisit[i], tmp);
      write( ' You visited '+tmp+' times' );
      writeln;

    end;

    writeln( 'Your choice ?' );
    readln(choice);

    if ( choice = 'q' ) then exit;
    if ( choice[1] in ['1'..'4'] ) then
    begin
      val( choice, i, err );
      placeVisit[i] := placeVisit[i] + 1;
      curPlace := 4 + ( i - 1 );

      pack := 'ishar';

      if ( i = 3 ) then
      begin
        curPlace := placeVisit[i];
        if ( curPlace > 4 ) then curPlace := 1;
        pack := 'game1-1';
      end;

    end;

    zz := zz+1;


  until ( choice = 'q' );
  }


  {caract[ 1 ] := 'mages'; 
  capas[ 1 ] := 'Tavern:2'; }

  initScenes;
  curPlace := 1;

  clrscr;
  drawPakImage(pakNam[ curPlace ], pakNum[ curPlace ], 0, 0);

  gotoXY(0,0);
  writeln( placeNam[ curPlace ] );
  writeln( txt[ curPlace ] );
  writeln( caract[ curPlace ] );
  writeln( capas[ curPlace ] );


end.
