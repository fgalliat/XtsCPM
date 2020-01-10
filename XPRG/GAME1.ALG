@Xtase - fgalliat Jul 2019

-= Game1 algorithm =-

1|ishar|6|In the castle|mages|The place to learn spells & skills...|Tavern:2
100|game1-1|1|The river|nobody|Everything looks so peacefull here, no sound... but...|Next:101
101|game1-1|1|The river|nobody|You suddenly hear a shy voice...|Next:110



1) reindex 1st col (here 1,100,101 would be mapped as 0,1,2) & keep indexes values
2) use 2nd col as PAK file name
3) it's the  #frameNum
4) the title of the Place
5) the name of present characters (comma separated)-(nobody is a reserved word)
6) the scene text (255 chars max)
7) Capabilities ex "Yes:200,No:1" (comma separated)-(re-use indexed values of 1st col)




