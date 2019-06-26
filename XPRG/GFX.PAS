program testPtr;

{ Xtase YATL demo @May 2019 }

{$I XTSYATLG.INC gfx control routines}
{$I XTSYATLC.INC console control routines}
{$I XTSYATLM.INC music control routines}

var
 str : string[64];
 choice : string[1];
 trk: integer;
 i: integer;
 choiceCh: char;

begin

 clScr; {* no args > no parent. *}
 drawBmp('girl.bmp');

 drawBmp('!sprite1.bmp'); {* loadSomeSprites *}
 defineSprite(0, 1, 0, 19, 19);

 drawSprite( 0, 10, 10 );
 i := 0;

 repeat 
   {readln( choice );}
   choiceCh := '*';
   if ( KeyPressed ) then 
     Read(Kbd,choiceCh);

   drawLine(10+(i*2),10+(i*2),100,50, i mod 9);
   drawRect(300-(i*2),220,15,15, 1, Random(9) );

   drawSprite( 0, 10+(i*2), 10+(i*2) );

   i:=i+1;
   if ( i > 50 ) then i := 0;
 
 until ( choiceCh = 'q' ); { or ... }

end.
