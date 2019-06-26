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

 x,y,w,h: integer;
begin

 clScr; {* no args > no parent. *}
 drawBmp('girl.bmp');

 drawBmp('!sprite1.bmp'); {* loadSomeSprites *}
 defineSprite(0, 0, 1, 19, 19); { upperL }
 defineSprite(1, 40, 1, 19, 19); { upperR }
 defineSprite(2, 40,20, 19, 19); { lowerR }

 defineSprite(5, 20, 1, 19, 19); { title bck }

 defineSprite(6, 62, 1, 31, 28); { file }
 defineSprite(7, 95, 1, 31, 28); { folder }

 w := 150;
 h := 100;
 x := 320-w-5;
 y := 240-h-20-5;

 drawRect(x+1,y+1,w,h, 1, 0 );
 drawRect(x,y,w,h, 1, 1 );
 
 for i:=0 to (w div 19) do
   drawSprite( 5, x+(i*19), y );

 drawSprite( 0, x, y );
 drawSprite( 1, x+w-19, y );
 drawSprite( 2, x+w-19, y+h-19 );

 drawSprite( 6, x+10, y+20+10 );
 drawSprite( 7, x+10 +32, y+20+10 );

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
