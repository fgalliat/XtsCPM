program JukeBox;

{ Xtase YAEL MP3 JukeBox @Sept 2019 }

var 
 i : integer;

{$I XTSYATLG.INC the Yatl GFX include}
{$I XTSYATLC.INC the Yatl TTY include}

{$I XTSYATLM.INC the Yatl MP3 include}

{ to put in include }
function doesMp3Play : boolean;
begin
 doesMp3Play := ( Bdos(227, 6 shl 8 ) ) = 1;
end;

{ to put in a generic include }
{ 'a' -> 1 }
function getChoiceNum(ch : char) : integer;
begin
  if ( ch = '' ) then getChoiceNum := 0;
  getChoiceNum := ord(ch) - ord('a') + 1;
end;


const
 nbTruc = 4;

var
 {pack : string[8];}
 choice : string[32];
 tmp: string[16];
 zz,err : integer;
 {curPlace : integer;}
 choiceCh : char;
begin

  stopMp3;

  clrscr;
  { auto-add '.pak' }
  drawPakImage('ishar', 6, 0, 0);
  {gotoXY(1,1); -- need to set TTYcurs }
  cursor(1,1);

  choice := '*';

  {* ------ *}
  playMp3( 22 );
  cursor(1,1);
  writeln( 'Xtase JukeBox v1.0' );
  cursor(1,3);
  writeln( 'Playing an MP3 : ' );

  repeat

    cursor(22,3); write('NO ');
    if ( doesMp3Play ) then
    begin
      cursor(22,3); write('YES');
    end;

    cursor(1,6);
    write( 'Your choice ? (q, p, n)' );
    if ( KeyPressed ) then 
     Read(Kbd,choiceCh);

    if ( choiceCh = 'p' ) then
      pauseMp3;
    if ( choiceCh = 'n' ) then
      nextMp3;

  until ( choiceCh = 'q' );

  stopMp3;

end.
