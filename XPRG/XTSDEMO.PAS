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
 str := 'Let s play an MP3 now !';

 setConsoleColors(1); {* LCD mono *}
 setConsoleCols(1); {* 40 cols *}
 clScr; {* no args > no parent. *}
 cursor( 10, 5 );
 writeln( 'How, I m a poor lcd mono display' );
 cursor( 10, 7 );
 writeln( 'Let s play with Buzzer' );
 cursor( 0, 9 );
 play('MONKEY.T5K'); {* wait keyPress *}

 setConsoleColors(0); {* phosphore mode *}
 clScr;
 drawBmp('girl.bmp');
 
 { drawRect(10,10,100,100, 1, 65535); }
 drawRect(280,10,30,30, 1, 8);
 drawCircle(280+15,10+15,15, 1, 3);
 for i := 0 to 30 do
 begin
   drawLine(10+(i*2),10+(i*2),100,50, i mod 9);
 end;

 cursor( 10, 5 );
 txtColor;
 writeln( 'Maybe a colorfull screen !!!!' );
 txtNormal;
 cursor( 10, 7 );
 writeln( str );

 { 60-duHast / 65-C64MENU }
 playMp3(60);

 cursor( 10, 9 );
 writeln( 'Choose Action (m,s,p,n,c,q) ...' );
 
 repeat 
   {readln( choice );}
   choiceCh := '*';
   if ( KeyPressed ) then 
     Read(Kbd,choiceCh);

   drawLine(10+(i*2),10+(i*2),100,50, i mod 9);
   drawRect(300,220,15,15, 1, i mod 9);

   i:=i+1;
   if ( i > 50 ) then i := 0;

   if ( choiceCh = 'm' ) then 
    playMp3(65);
   if ( choiceCh = 's' ) then
    stopMp3;
   if ( choiceCh = 'p' ) then
    pauseMp3;
   if ( choiceCh = 'n' ) then
    nextMp3;
   if ( choiceCh = 'c' ) then
    begin
     writeln('TrackNum ?');
     readln(trk);
     playMp3(trk);
    end;
 until ( choiceCh = 'q' ); { or ... }

 setConsoleCols(0); {* 80 cols *}
 clScr;
 writeln('Bye.');
 stopMp3;
end.
