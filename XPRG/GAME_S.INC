{.PA}
{ Xtase Adventure Game1 }
{ Xtase - fgalliat @ Jul 2019 }

const
   nbScene=12;
var 
  placeNam : array[1..nbScene] of string[32]; 
  pakNam   : array[1..nbScene] of string[8]; 
  pakNum   : array[1..nbScene] of integer; 
  txt      : array[1..nbScene] of string[255]; 
  caract   : array[1..nbScene] of string[32]; 
  capas    : array[1..nbScene] of string[32]; 


procedure initScenes;
begin
  placeNam[ 1 ] := 'In the castle'; 
  pakNam[ 1 ] := 'ishar'; 
  pakNum[ 1 ] := 6; 
  txt[ 1 ] := 'The place to learn spells & skills...'; 
  caract[ 1 ] := 'mages'; 
  capas[ 1 ] := 'Tavern:2'; 

  placeNam[ 2 ] := 'In the tavern'; 
  pakNam[ 2 ] := 'ishar'; 
  pakNum[ 2 ] := 7; 
  txt[ 2 ] := 'The place where everybody is nobody'; 
  caract[ 2 ] := 'servant'; 
  capas[ 2 ] := 'Servant:3,Castle:1'; 

  placeNam[ 3 ] := 'In the tavern'; 
  pakNam[ 3 ] := 'ishar'; 
  pakNum[ 3 ] := 7; 
  txt[ 3 ] := 'Hey guy, you look tied !, let s drink something'; 
  caract[ 3 ] := 'servant'; 
  capas[ 3 ] := 'Servant:3,Castle:1,River:4'; 

  placeNam[ 4 ] := 'The river'; 
  pakNam[ 4 ] := 'game1-1'; 
  pakNum[ 4 ] := 1; 
  txt[ 4 ] := 'Everything looks so peacefull here, no sound... but...'; 
  caract[ 4 ] := 'nobody'; 
  capas[ 4 ] := 'Next:5'; 

  placeNam[ 5 ] := 'The river'; 
  pakNam[ 5 ] := 'game1-1'; 
  pakNum[ 5 ] := 1; 
  txt[ 5 ] := 'You suddenly hear a shy voice...'; 
  caract[ 5 ] := 'nobody'; 
  capas[ 5 ] := 'Next:6'; 

  placeNam[ 6 ] := 'The river'; 
  pakNam[ 6 ] := 'game1-1'; 
  pakNum[ 6 ] := 2; 
  txt[ 6 ] := 'Hi, I am Candice, Who are you ?'; 
  caract[ 6 ] := 'Candice'; 
  capas[ 6 ] := 'Warrior:7,Stranger:8'; 

  placeNam[ 7 ] := 'The river'; 
  pakNam[ 7 ] := 'game1-1'; 
  pakNum[ 7 ] := 2; 
  txt[ 7 ] := 'Oh... I hate wars, see you !'; 
  caract[ 7 ] := 'Candice'; 
  capas[ 7 ] := 'Next:4'; 

  placeNam[ 8 ] := 'The river'; 
  pakNam[ 8 ] := 'game1-1'; 
  pakNum[ 8 ] := 2; 
  txt[ 8 ] := 'Oh... You know, I m not born here too !'; 
  caract[ 8 ] := 'Candice'; 
  capas[ 8 ] := 'Next:9'; 

  placeNam[ 9 ] := 'The river'; 
  pakNam[ 9 ] := 'game1-1'; 
  pakNum[ 9 ] := 2; 
  txt[ 9 ] := 'Would you like to meet my friend Okuni ?'; 
  caract[ 9 ] := 'Candice'; 
  capas[ 9 ] := 'Yes:12,No:10'; 

  placeNam[ 10 ] := 'The river'; 
  pakNam[ 10 ] := 'game1-1'; 
  pakNum[ 10 ] := 2; 
  txt[ 10 ] := 'Really ?, As you wish...'; 
  caract[ 10 ] := 'Candice'; 
  capas[ 10 ] := 'Yes:12,No:6'; 

  placeNam[ 11 ] := 'The river'; 
  pakNam[ 11 ] := 'game1-1'; 
  pakNum[ 11 ] := 3; 
  txt[ 11 ] := 'Seems that Candice has went...'; 
  caract[ 11 ] := 'Okuni'; 
  capas[ 11 ] := 'Hmmm:11'; 

  placeNam[ 12 ] := 'The river'; 
  pakNam[ 12 ] := 'game1-1'; 
  pakNum[ 12 ] := 4; 
  txt[ 12 ] := 'Hi, I m Okuni, the Candice s friend'; 
  caract[ 12 ] := 'Candice,Okuni'; 
  capas[ 12 ] := 'Hi:11'; 

end;
