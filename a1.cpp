// CS 349 Fall 2018
// A1: Breakout code sample
// You may use any or all of this code in your assignment!
// See makefile for compiling instructions

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

// handy struct to save display, window, and screen
struct XInfo
{
	Display *display;
	int screen;
	Window window;
	Pixmap buffer;
	GC gc;
};

// class definition for brick
class Brick
{
	int x;
	int y;
	int height;
	int width;
	bool hit;
	unsigned long int pixel;

  public:
	Brick(int x, int y, int height, int width, unsigned long int pixel) : x(x), y(y), height(height), width(width), pixel(pixel){
		hit = false;
	}

	Brick() {}

	int getX()
	{
		return x;
	}

	int getY()
	{
		return y;
	}

	int getHeight()
	{
		return height;
	}

	int getWidth()
	{
		return width;
	}

	bool getHit() {
		return hit;
	}

	void setHit(bool hit) {
		this->hit = hit;
	}

	unsigned long int getColor() {
		return this->pixel;
	}

	void paint(XInfo xinfo, Colormap cmap)
	{
		if (!hit) {
			XSetForeground(xinfo.display, xinfo.gc, pixel);
			XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc, x, y, width, height);
		}
	}
};

// class definition for ball
class Ball
{
	XPoint pos;
	XPoint dir;
	unsigned long int pixel;
	int size;

  public:
	Ball(int posX, int posY, int dirX, int dirY, int size, unsigned long int pixel)
	{
		pos.x = posX;
		pos.y = posY;
		dir.x = dirX;
		dir.y = dirY;
		this->size = size;
		this->pixel = pixel;
	}

	XPoint getPos()
	{
		return pos;
	}

	XPoint getDir()
	{
		return dir;
	}

	int getSize()
	{
		return size;
	}

	void setPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
	}

	void setDir(int x, int y)
	{
		dir.x = x;
		dir.y = y;
	}

	unsigned long int getColor() {
		return this->pixel;
	}

	void setColor(unsigned long int color) {
		pixel = color;
	}

	void paint(XInfo xinfo, Colormap cmap)
	{
		XSetForeground(xinfo.display, xinfo.gc, pixel);
		XFillArc(xinfo.display, xinfo.buffer, xinfo.gc,
				 this->pos.x - this->size/2,
				 this->pos.y - this->size/2,
				 this->size, this->size,
				 0, 360 * 64);
	}
};

// check if all the bricks are cleared
void checkClear(Brick bricks[][10], bool & isClear) {
	bool clear = true;
	if (!isClear) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 10; j++) {
				if (!bricks[i][j].getHit()) {
					clear = false;
				}
			}
		}
		if (clear) isClear = true;
	}
}

// check if the ball hits a brick
void collision(Ball &ball, Brick &brick, int &score)
{
	bool top = false;
	bool bottom = false;
	bool left = false;
	bool right = false;

	if (!brick.getHit()) {
		int xR = brick.getX() + brick.getWidth();
		int yB = brick.getY() + brick.getHeight();
		// hit on the left
		if (ball.getPos().x + ball.getDir().x + ball.getSize() / 2 > brick.getX() &&
			ball.getPos().x + ball.getDir().x + ball.getSize() / 2 < xR &&
			ball.getPos().y >= brick.getY() && ball.getPos().y <= yB)
		{
			left = true;
		}
		//hit on the right
		if (ball.getPos().x + ball.getDir().x - ball.getSize() / 2 > brick.getX() &&
			ball.getPos().x + ball.getDir().x - ball.getSize() / 2 < xR &&
			ball.getPos().y >= brick.getY() && ball.getPos().y <= yB)
		{
			right = true;
		}
		// hit on the top
		if (ball.getPos().y + ball.getDir().y + ball.getSize() / 2 > brick.getY() &&
			ball.getPos().y + ball.getDir().y + ball.getSize() / 2 < yB &&
			ball.getPos().x >= brick.getX() && ball.getPos().x <= xR)
		{
			top = true;
		}
		// hit on the bottom
		if (ball.getPos().y + ball.getDir().y - ball.getSize() / 2 > brick.getY() &&
				 ball.getPos().y + ball.getDir().y - ball.getSize() / 2 < yB &&
				 ball.getPos().x >= brick.getX() && ball.getPos().x <= xR)
		{
			bottom = true;
		}

		if (left || right) {
			ball.setDir(-ball.getDir().x, ball.getDir().y);
			brick.setHit(true);
			ball.setColor(brick.getColor());
		}
		if (top || bottom) {
			ball.setDir(ball.getDir().x, -ball.getDir().y);
			brick.setHit(true);
			ball.setColor(brick.getColor());
		}
		// if hit a brick, update the score
		if (brick.getHit()) {
			score += 1;
		}
	}
	
}

// Initialize X and create a window
void initX(int argc, char *argv[], XInfo &xinfo)
{
	// create window
	xinfo.display = XOpenDisplay("");
	if (xinfo.display == NULL)
		exit(-1);
	xinfo.screen = DefaultScreen(xinfo.display);
	long background = WhitePixel(xinfo.display, xinfo.screen);
	long foreground = BlackPixel(xinfo.display, xinfo.screen);
	xinfo.window = XCreateSimpleWindow(xinfo.display, DefaultRootWindow(xinfo.display),
								 10, 10, 1280, 800, 2, foreground, background);

	// disable window resizing
	XSizeHints *sizeHint = XAllocSizeHints();
	sizeHint->flags = PMaxSize | PMinSize;
	sizeHint->max_height = 800;
	sizeHint->min_height = 800;
	sizeHint->max_width = 1280;
	sizeHint->min_width = 1280;
	XSetNormalHints(xinfo.display, xinfo.window, sizeHint);

	// set events to monitor and display window
	XSelectInput(xinfo.display, xinfo.window, ButtonPressMask | KeyPressMask);
	XMapRaised(xinfo.display, xinfo.window);
	XFlush(xinfo.display);
}

// default frames per second animation
int FPS = 60;

// default ball speed
int ballSpeed = 3;

// get current time
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

///////////////////////////////////////////////////////////////////////////////
// entry point
int main( int argc, char *argv[] ) {

	// check if correct arguments are passed
	if (argc != 3) {
		cout << "arguments number incorrect, expecting FPS and ball speed" << endl;
		return 1;
	}
	string fps(argv[1]);
	string speed(argv[2]);
	istringstream iss_fps(fps);
	istringstream iss_speed(speed);
	iss_fps >> FPS;
	iss_speed >> ballSpeed;
	// check if arguments are valid
	if (FPS < 10 || FPS > 60) {
		cout << "invalid FPS passed" << endl;
		return 1;
	}
	if (ballSpeed < 1 || ballSpeed > 10)
	{
		cout << "invalid ball speed passed" << endl;
		return 1;
	}

	// array that stores the customized colors
	XColor xcolor[5];
	for (int i = 0; i < 5; i++)
	{
		xcolor[i].flags = DoBlue | DoRed | DoGreen;
	}

	// set the colors
	xcolor[0].red = 32000;
	xcolor[0].green = 64000;
	xcolor[0].blue = 32000;

	xcolor[1].red = 16000;
	xcolor[1].green = 32000;
	xcolor[1].blue = 32000;

	xcolor[2].red = 32000;
	xcolor[2].green = 32000;
	xcolor[2].blue = 64000;

	xcolor[3].red = 32000;
	xcolor[3].blue = 32000;
	xcolor[3].green = 16000;

	xcolor[4].red = 64000;
	xcolor[4].green = 32000;
	xcolor[4].blue = 32000;

	XInfo xinfo;

	initX(argc, argv, xinfo);

	// count the score
	int score = 0;

	// lives
	int health = 2;

	// track if the game is over
	bool isPreparing = true;
	bool isGameOver = false;
	bool isClear = false;

	// allocate colors
	Colormap cmap = DefaultColormap(xinfo.display, DefaultScreen(xinfo.display));
	for (int i = 0; i < 5; i++) {
		XAllocColor(xinfo.display, cmap, &xcolor[i]);
	}
	
	// create bricks
	Brick bricks[5][10];
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			bricks[i][j] = Brick(50+120*j, 50+70*i, 50, 100, xcolor[i].pixel);
		}
	}

	// block position, size
	XPoint rectPos;
	XPoint rectPrev;
	rectPos.x = 640;
	rectPos.y = 750;
	unsigned long int rectColor = WhitePixel(xinfo.display, DefaultScreen(xinfo.display));

	Ball ball = Ball(rectPos.x + 50, rectPos.y - 5, 0, 0, 10, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));

	// create gc for drawing
	xinfo.gc= XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XWindowAttributes w;
	XGetWindowAttributes(xinfo.display, xinfo.window, &w);

	// save time of last window paint
	unsigned long lastRepaint = 0;

	// save time of last computation
	unsigned long lastCompute = 0;

	// event handle for current event
	XEvent event;

	// DOUBLE BUFFER
	// create bimap (pximap) to us a other buffer
	int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
	xinfo.buffer = XCreatePixmap(xinfo.display, xinfo.window, w.width, w.height, depth);

	// event loop
	while ( true ) {

		// process if we have any events
		if (XPending(xinfo.display) > 0) { 
			XNextEvent( xinfo.display, &event ); 

			switch ( event.type ) {

				case KeyPress: // any keypress
					KeySym key;
					char text[10];
					int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );

					if (i == 1 && text[0] == ' ') {
						// press space to start the game
						if (isPreparing) {
							ball.setDir(ballSpeed, -ballSpeed);
							isPreparing = false;
						}
					}

					if (!isGameOver) {
						// move right
						if ( i == 1 && text[0] == 'd' ) {
							if (rectPos.x < 1180) {
								if (isPreparing) ball.setPos(ball.getPos().x + 10, ball.getPos().y);
								rectPos.x += 10;
							}
						}

						// move left
						if ( i == 1 && text[0] == 'a' ) {
							if (rectPos.x > 0) {
								if (isPreparing) ball.setPos(ball.getPos().x - 10, ball.getPos().y);
								rectPos.x -= 10;
							}
						}
					} else {
						// play again
						if ( i == 1 && text[0] == 'p' ) {
							// set the game status to preparing
							isPreparing = true;
							// reset the score
							score = 0;
							// reset the health
							health = 2;
							// reset the paddle
							rectPos.x = 640;
							rectPos.y = 750;
							rectColor = WhitePixel(xinfo.display, DefaultScreen(xinfo.display));
							// reset the ball
							ball.setPos(rectPos.x + 50, rectPos.y - 5);
							ball.setDir(0, 0);
							ball.setColor(BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
							// reset all the bricks
							for (int i = 0; i < 5; i++) {
								for (int j = 0; j < 10; j++) {
									bricks[i][j].setHit(false);
								}
							}

							isGameOver = false;
						}
					}
					

					// quit game
					if ( i == 1 && text[0] == 'q' ) {
						XCloseDisplay(xinfo.display);
						exit(0);
					}
					break;
				}
		}

		unsigned long end = now();	// get current time in microsecond

		if (end - lastRepaint > 1000000 / FPS) {
			// draw into the buffer
			// note that a window and a pixmap are “drawables”
			XSetForeground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, DefaultScreen(xinfo.display)));
			XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc,
						   0, 0, w.width, w.height);

			XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
			if (!isGameOver) {

				// all bricks cleared, display a message and wait for new bricks
				if (isClear) {
					string clearText = "All bricks cleared. New bricks will be generated after the next hit on the paddle.";
					XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width / 2 - 250, w.height / 2,
								clearText.c_str(), clearText.length());
				}
				
				// if the game is not started, print student name, id, and instructions.
				if (isPreparing) {
					string nameText = "Name: Sicheng Dai";
					string idText = "User id: s8dai";
					string insText = "Press 'a' to move the paddle left, press 'd' to move the paddle right, or press space to start the game.";
					XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width / 2, w.height / 2 + 50,
								nameText.c_str(), nameText.length());
					XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width / 2, w.height / 2 + 70,
								idText.c_str(), idText.length());
					XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width / 2 - 250, w.height / 2 + 90,
								insText.c_str(), insText.length());
				}

				// draw the score, fps, and health at the top left corner
				string scoreText = "Score: " + to_string(score);
				string fpsText = "FPS: " + to_string(FPS);
				string healthText = "Health: "+ to_string(health);
				XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, 10, 10, fpsText.c_str(), fpsText.length());
				XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, 10, 20, scoreText.c_str(), scoreText.length());
				XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, 10, 30, healthText.c_str(), healthText.length());

				// draw the paddle
				XSetForeground(xinfo.display, xinfo.gc, rectColor);
				XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc, rectPos.x, 
					rectPos.y, 100, 5);
				XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
				XDrawRectangle(xinfo.display, xinfo.buffer, xinfo.gc, rectPos.x, 
					rectPos.y, 100, 5);

				
				// draw bricks
				for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 10; j++) {
						bricks[i][j].paint(xinfo, cmap);
					}
				}

				// draw ball from centre
				ball.paint(xinfo, cmap);
				
				
				
			} else {
				// game over, display score, and give instructions
				string gameOverText = "Game Over! Your final score is " + to_string(score) + ".";
				string gameOVerMenuText = "Press 'p' to play again, or press 'q' to quit.";
				XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width/2 - 100, w.height/2, 
				gameOverText.c_str(), gameOverText.length());
				XDrawString(xinfo.display, xinfo.buffer, xinfo.gc, w.width/2 - 100, w.height/2 + 50, 
				gameOVerMenuText.c_str(), gameOVerMenuText.length());
			}

			XCopyArea(xinfo.display, xinfo.buffer, xinfo.window, xinfo.gc,
					  0, 0, w.width, w.height, // region of pixmap to copy
					  0, 0);				   // position to put top left corner of pixmap in window

			XFlush( xinfo.display );

			lastRepaint = now(); // remember when the paint happened
		}

		end = now();

		if (end - lastCompute > 1000000 / 60) {

			// check if all bricks are cleared
			checkClear(bricks, isClear);

			// update ball position
			ball.setPos(ball.getPos().x + ball.getDir().x,
						ball.getPos().y + ball.getDir().y);

			//bounce ball
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					collision(ball, bricks[i][j], score);
				}
			}

			// the ball hits the left or right boundary
			if (ball.getPos().x + ball.getDir().x + ball.getSize() / 2 > w.width ||
				ball.getPos().x + ball.getDir().x - ball.getSize() / 2 < 0)
				ball.setDir(-ball.getDir().x, ball.getDir().y);
			// the ball hits the bottom, game over
			if (ball.getPos().y + ball.getDir().y + ball.getSize() / 2 > w.height) {
				if (health > 0) {
					health --;
					ball.setDir(ball.getDir().x, -ball.getDir().y);
				} else if (health == 0) {
					isGameOver = true;
				}
				
				if (isClear)
				{
					isClear = false;
					for (int i = 0; i < 5; i++)
					{
						for (int j = 0; j < 10; j++)
						{
							bricks[i][j].setHit(false);
						}
					}
				}
			}	
			// the ball hits the top of the screen
			if (ball.getPos().y + ball.getDir().y - ball.getSize() / 2 < 0)
				ball.setDir(ball.getDir().x, -ball.getDir().y);
			// the ball hits the paddle
			if (ball.getPos().y + ball.getSize() / 2 == rectPos.y &&
				ball.getPos().x >= rectPos.x - 5 &&
				ball.getPos().x <= rectPos.x + 105 && ball.getDir().y > 0) {
					ball.setDir(ball.getDir().x, -ball.getDir().y);
					if (!isPreparing)
						rectColor = ball.getColor();
					if (isClear) {
						isClear = false;
						for (int i = 0; i < 5; i++) {
							for (int j = 0; j < 10; j++) {
								bricks[i][j].setHit(false);
							}
						}
					}
				}
				

			lastCompute = now();
		}

		// IMPORTANT: sleep for a bit to let other processes work
		if (XPending(xinfo.display) == 0) {
			usleep(1000000 / 60 - (now() - lastCompute));
		}
	}
	XCloseDisplay(xinfo.display);
}
