program testSx;

{ Xtase YATL demo @Jul 2019 }

{$I XTSYATLG.INC gfx control routines}
{$I XTSYATLC.INC console control routines}
{$I XTSYATLM.INC music control routines}

var
 str : string[64];
 choice : string[1];
 trk: integer;
 i: integer;
 choiceCh: char;

 x,y,w,h: integer;
begin

 clScr; {* no args > no parent. *}
 drawBmp('girl.bmp');

 drawBmp(#1+'pack1.pak'); {* loadSomePackedImg *}
 w := 150;
 h := 100;
 x := 320-w-5;
 y := 240-h-20-5;

 i := 0;

 repeat 
   {readln( choice );}
   choiceCh := '*';
   if ( KeyPressed ) then 
     Read(Kbd,choiceCh);
   drawLine(10+(i*2),10+(i*2),100,50, i mod 9);
{
   drawRect(300-(i*2),220,15,15, 1, Random(9) );
}
   i:=i+1;
   if ( i > 50 ) then i := 0;
 
 until ( choiceCh = 'q' ); { or ... }

end.
