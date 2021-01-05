# CrazyWorm
Simple test game for blinks
This is my first test game for blinks and my first time with arduino so I now the code must be much more better than is right now

CrazyWorm is a (competitive/solo) game where you have to drive your worm the fastest way you can to the finish line.
The worm has a fece where random colors appear and a back where only one color shows and you have to move the back to the face to move forward.
The color on the back is a random pick from the 3 face colors: front, left, right. You can rotate those colors according the chances you configure previous the game.

Setup
5 blinks in a row. Long press to restart the game, when all blinks turn green we are ready. The result:

![alt text](https://github.com/jkarpago/CrazyWorm/blob/main/img/setup.jpg?raw=true)

The sixth blink will be the finish line and the timeout clock. Long press to turn green and then just single click to select the minutes on the clock (1 to 6). Double click and the clock will change the color 3 times before start the countdown. Each minute has a color (1:RED, 2:ORANGE, 3:GREEN, 4:YELLOW, 5:BLUE, 6:WHITE) and each face of the blink on is 10 seconds of that minute.

Change number selection and start
We have the front and the back of the row. Single click in the front (the blink closest to finish line) and one fece will turn blue. Each time you click one more face will be blue. These number of blue faces are the extra changes (0 to 6) you have to change your turn (explanation in a second).

![alt text](https://github.com/jkarpago/CrazyWorm/blob/main/img/changes.jpg?raw=true)

Once you have configured the extra changes just double click and the face blink will change all the faces colors, the middle blinks of the worm will follow the face color and the last blink tell us what color we have to combine with the face color:

![alt text](https://github.com/jkarpago/CrazyWorm/blob/main/img/start.jpg?raw=true)

Each player must separate the back blink and union the face color with the correspondent face color in the front blink. If you unite both faces succesfully the blink turn green and the worm change colors to unite the back and front again. The unite colors in the face are allways the oposite, left and right to the side face unite to previous blink. You can change the order of the colors if you make a single click to the front blink, in case you are not happy with the color you need in the position you have. You can change this as many times as you have available according to the number of extra changes you select in setup and changes you used before. When all your changes are gone the blink will turn red for one second. If you want to know how many changes you have available just 3 clicks in the blink. Sometimes the last blink does not fill ok the color (I am investigating why and hope I fix it soon), in that case you can reset the worm with double click in the front, just the way you did to start.

Sometimes the blinks go out of the table, no table is infinite, or you and your opponents are left of space in that case the players can move their blinks left or right (not forward, no cheat please).

Goal
The first player to reach the finish line wins
If the clock is over the player closest to finish line wins
If one player goes back to the start line loses
