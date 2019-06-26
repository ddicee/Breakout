Welcome to Sicheng Dai's breakout game.

Basic operations:
	press 'a' to move the paddle left
	press 'd' to move the paddle right
	press space to launch the ball (start the game)
	press 'p' at the game over page to restart the game
	press 'q' to quit during the game

Features and specification:
	1. At the beginning of each cycle of the game, the player's health is 2.
Each time the ball hits the bottom of the display, player's health is decremented
by 1, and the ball is bounced back regardless of the position of the paddle 
(**it will cross the paddle if the paddle blocks its way back**). As soon as 
player's health falls below 0, the game is over.

	2. When the game is in preparation (the ball is not launched), the player can
adjust the position of the paddle, which is the same as the launching point of the
ball. Once the player presses space, the ball will be launched and the instructions
on the display will disappear.

	3. Each time the ball hits a brick, the player's score will be adde by 1.

	4. Usually when the ball hits a brick, either its x-axis speed or y-axis 
speed will be changed based on which side the brick is hit. However, when the ball hits
exactly on the corner (which is rare), both x-axis and y-axis speed will be changed.

	5. When the ball hits a brick, the ball's color will be changed to the same as
the hit brick's color. Also, when the ball hits the paddle, it will pass its color
to the paddle. The ball is initially black, and paddle is initially white.
	
	6. When all the bricks are cleared, a waiting message will be displayed 
on the screen. As soon as the ball hits the paddle or the bottom of the display
(in which case the player's health will be decreased), the message will disappear,
and 5 rows of new bricks will be generated on the display. The score will continue
counting.

	7. If the game is over, some text will be displayed to let the player quit the
game or restart the game, as well as the final score. Player can choose to press 'p'
 to restart or press 'q' to quit. 

 
 Hope you will enjoy the game!
 
 Thanks,
 Sicheng Dai
