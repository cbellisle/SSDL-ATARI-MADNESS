//2D breakout just because
//caleb bellisle
//started 7/1/2022

#include <fstream>
#include "SSDL.h"
#include <iostream>

/*
log starting 7/14
good progress today, worked on colors, got the full game loop working

need to work on;

start screen needs polishing


lives DONE

end screen

powerups?
music ehhhhhhhh

LEVELS(generated?)
stuff

*/

enum {EMPTY, BRICK, BLOCK, PADDLE };

enum { COLS = 15, ROWS = 16, BLOCKW = 50, BLOCKH = 30, BALLRADIUS = 10, PADDLEH = BLOCKH/2, PADDLEW = BLOCKW*2 };

//bad variables but its fine
int WINDOWW = COLS * BLOCKW + COLS -15;
// *2 because we want there to be room for the player
int WINDOWH = ((ROWS/2) * BLOCKH + (ROWS/2)) * 2;

int currentScore = 0;

struct Ball
{//first push
	double xVelo = 0;
	double yVelo = 0;
	double xPos = -1;
	double yPos = -1;

	int currentX;
	int currentY;

};

struct Paddle
{
	double xPos;
	double yPos;
	int currentX;
	int currentY;
};

SSDL_Color newColor(int r, int g, int b);

Ball move(int level[ROWS][COLS], Ball ball, Paddle paddle, int& nBricks);

Paddle movePaddle(int level[ROWS][COLS], Paddle paddle);


int loadLevel(int level[ROWS][COLS]);


void drawBox(int level[ROWS][COLS], int colors[ROWS * COLS], Ball ball);

void drawPaddle(int level[ROWS][COLS], Paddle paddle);

void drawBall(int level[ROWS][COLS], Ball ball);

void displayLives(int lives);

void generateColors(int colors[ROWS * COLS]);

int getScore();



int main(int argc, char** argv)
{
	SSDL_SetWindowSize(WINDOWW, WINDOWH);
	srand(time(nullptr));

	int currentLevel[ROWS][COLS];

	int colors[ROWS * COLS];

	Ball ball;
	Paddle paddle;

	int nLives = 3;
	bool playing = true;
	bool ballStuck = true;
	while (1)
	{
		currentScore = 0;

		int nBlocks = loadLevel(currentLevel);

		generateColors(colors);

		SSDL_Image BACKGROUND = SSDL_LoadImage("media/background.jfif");

		paddle.xPos = (double)WINDOWW / 2 - 30;
		paddle.yPos = (double)WINDOWH - 45;

		ball.xPos = paddle.xPos + (PADDLEW / 2);
		ball.yPos = paddle.yPos - 10;

		ball.xVelo = 0;
		ball.yVelo = 0;

		ballStuck = true;
		playing = true;

		drawBox(currentLevel,colors, ball);
		drawBall(currentLevel, ball);


		SSDL_SetFramesPerSecond(60);

		//initial loop, runs during the whole program, main menu/game/end/extras all run here
		while (playing)
		{
			//display stats at top, points/level/lives/quit
			SSDL_SetCursor(5, 10);
			SSDL_SetRenderDrawColor(BLACK);

			sout << "Score: " << currentScore << "\t\t\t\tLevel 1";

			displayLives(nLives);



			if (ballStuck)
			{//anything in here will be displayed before the game begins
				SSDL_SetCursor(WINDOWW / 2, 400);
				SSDL_SetRenderDrawColor(WHITE);
				sout << "Press 'a' and 'd' to move the paddle\n\t\t\t\tPress Space to launch the ball!";
				if (SSDL_IsKeyPressed(' '))
				{
					ballStuck = false;
					ball.xVelo = 3;
					ball.yVelo = -3;
				}
			}
				
			//calculate locations of 
			paddle.currentX = (int)paddle.xPos / 50;
			paddle.currentY = (int)paddle.yPos / 30;
			ball.currentX = (int)ball.xPos / 50;
			ball.currentY = (int)ball.yPos / 30;



			//if(checkCollisions(currentLevel, ball))
			//	ball = reverse(ball);
			//if (checkCollisions(currentLevel, ball))
			SSDL_DefaultEventHandler();


			


			if (SSDL_IsKeyPressed('a'))
			{
				if(paddle.xPos >= 0)
					paddle.xPos -= 10;
				if(ballStuck == true)
					ball.xPos = paddle.xPos +(PADDLEW/2);
			}
			else if (SSDL_IsKeyPressed('d'))
			{
				if (paddle.xPos+PADDLEW <= WINDOWW)
					paddle.xPos += 10;

				if (ballStuck == true)
					ball.xPos = paddle.xPos+(PADDLEW / 2);
			}

			/*debugging stuff to see coords
			SSDL_SetRenderDrawColor(WHITE);
			SSDL_SetCursor(20, 450);

			sout << ball.xPos << " " << ball.yPos << "\t";

			sout << ball.currentX << " " << ball.currentY << "\n";

			sout << paddle.xPos << " " << paddle.yPos << " " << paddle.currentX << " " << paddle.currentY;
			*/

			drawBox(currentLevel,colors, ball);

			drawBall(currentLevel, ball);

			drawPaddle(currentLevel, paddle);

			ball = move(currentLevel, ball, paddle, nBlocks);

			if (ball.yPos > WINDOWH)
			{
				nLives--;
				paddle.xPos = (double)WINDOWW / 2 - 30;
				paddle.yPos = (double)WINDOWH - 45;

				ball.xPos = paddle.xPos + (PADDLEW / 2);
				ball.yPos = paddle.yPos - 10;

				ball.xVelo = 0;
				ball.yVelo = 0;
				ballStuck = true;
			}

			if (nLives < 0 || nBlocks <= 0)
				playing = false;


			SSDL_RenderClear();


			SSDL_RenderImage(BACKGROUND, 0, 0, WINDOWW, WINDOWH);
			//game loop in here




			//end loop

			//etc

		}
	}


	//main game loop

	SSDL_WaitKey();



	return 0;
}

int loadLevel(int level[ROWS][COLS])
{
	std::ifstream file("levels/level1.txt");

	int result = 0;
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			file >> level[i][j];
			if (level[i][j] == BLOCK)
				result++;
		}
	}
	return result;
}

void drawBox(int level[ROWS][COLS],int colors[ROWS * COLS], Ball ball)
{
	//SSDL_Image iBRICK = SSDL_LoadImage("media/block_solid.png");
	//SSDL_Image iBLOCK = SSDL_LoadImage("media/block.png");

	//x
	for (int i = 0; i < ROWS; i++)
	{//y
		for (int j = 0; j < COLS; j++)
		{
			//SSDL_RenderFillRect(j * 30, i * 30, 29, 29);


			if (level[i][j] == BRICK)
			{
				SSDL_SetRenderDrawColor(RED);
				SSDL_RenderFillRect(j * BLOCKW, i * BLOCKH, BLOCKW-1, (BLOCKH/5)-1);			// -1 so theres a gap between blocks
				SSDL_RenderFillRect(j * BLOCKW, i * BLOCKH +(BLOCKH -(BLOCKH/5)), BLOCKW - 1, (BLOCKH / 5) - 1);			// -1 so theres a gap between blocks

				SSDL_SetRenderDrawColor(WHITE);
				SSDL_SetCursor(j * BLOCKW, i * BLOCKH);
				SSDL_SetRenderDrawColor(BLACK);

				//sout << "BRICK";
				SSDL_SetRenderDrawColor(WHITE);


			}
			else if (level[i][j] == BLOCK)
			{
				
				SSDL_SetRenderDrawColor(newColor(0, 50, (50 + colors[i * j])));
				//SSDL_RenderImage(iBLOCK, j * BLOCKW, i * BLOCKH, BLOCKW-1, BLOCKH-1);
				SSDL_RenderFillRect(j * BLOCKW, i * BLOCKH, BLOCKW - 1, BLOCKH - 1);			// -1 so theres a gap between blocks
				SSDL_SetRenderDrawColor(WHITE);
				SSDL_SetCursor(j * BLOCKW, i * BLOCKH);
				//sout << "BLOCK";

			}
			else if (level[i][j] == EMPTY)
			{
				if (i < 8)
				{
					SSDL_SetRenderDrawColor(WHITE);
					SSDL_SetCursor(j * BLOCKW, i * BLOCKH);
					//sout << "EMPTY";
				}
				
			}

			
		}

	}
	
}

Ball move(int level[ROWS][COLS], Ball ball, Paddle paddle, int& nBricks)
{
	//controls the ball coords
	// and all ball collisions
	//delay of 0 so it doesnt kill itself
	SSDL_Delay(0);
	//update ball x and y
	ball.yPos += ball.yVelo;		
	ball.xPos += ball.xVelo;

	//if ball is going right
	if (ball.xVelo < 0)
	{	//check for x being out of bounds on left

		if (ball.xPos <= BALLRADIUS)
		{
			ball.xVelo *= -1;
			ball.xPos = (double)BALLRADIUS + 1;
		}
	}
	//if ball if going left
	else
	{	//checking for the x being out of bounds on right

		if (ball.xPos >= WINDOWW - (double)BALLRADIUS)
		{
			ball.xVelo *= -1;
			ball.xPos = WINDOWW - (double)BALLRADIUS - 1;
		}

	} 

	//check if ball is over the top of map
	if (ball.yPos <= 0 + BALLRADIUS)
	{
		ball.yVelo *= -1;
		ball.yPos = (double)BALLRADIUS + 1;
	}



	//block collisions
	//detect collisions. going to have to detect each side of the block
	//to determine to go left/right and up/down
	if (level[ball.currentY][ball.currentX] == BLOCK)
	{	//if ball is going down
		if (ball.yVelo > 0)
		{
			ball.yVelo *= -1;
			ball.yPos -= BALLRADIUS;
			level[ball.currentY][ball.currentX] = EMPTY;
			getScore();

		}//if ball is going up
		else
		{
			ball.yVelo *= -1;
			ball.yPos += BALLRADIUS;
			level[ball.currentY][ball.currentX] = EMPTY;
			getScore();

		}
		nBricks--;
	
	}
	//brick collisions
	if (level[ball.currentY][ball.currentX] == BRICK)
	{	//if ball is going down
		if (ball.yVelo > 0)
		{
			ball.yVelo *= -1;
			ball.yPos -= BALLRADIUS;
		} //if ball is going up
		else
		{
			ball.yVelo *= -1;
			ball.yPos += BALLRADIUS;
		}

	}
	//check if ball is hitting paddle based on x/y coords
	if (ball.yPos >= (paddle.yPos-BALLRADIUS) && ball.yPos <= paddle.yPos + BALLRADIUS)
		if(ball.xPos >= paddle.xPos && ball.xPos <= (paddle.xPos+ PADDLEW))
		{
			if (ball.xPos > paddle.xPos + (PADDLEW / 2) && ball.xVelo < 0)
				ball.xVelo *= -1;
			else if(ball.xPos < paddle.xPos + (PADDLEW / 2) && ball.xVelo > 0)
				ball.xVelo *= -1;

		ball.yVelo *= -1;
		ball.yPos -= BALLRADIUS;
		}
	
	return ball;

}



void drawBall(int level[ROWS][COLS], Ball ball)
{
	SSDL_SetRenderDrawColor(RED);
	SSDL_RenderFillCircle(ball.xPos, ball.yPos, BALLRADIUS);
}


Paddle movePaddle(int level[ROWS][COLS], Paddle paddle)
{
	return paddle;

}

void drawPaddle(int level[ROWS][COLS], Paddle paddle)
{
	SSDL_Image PADDLE = SSDL_LoadImage("media/paddle.png");
	
	SSDL_RenderImage(PADDLE, paddle.xPos, paddle.yPos, PADDLEW, PADDLEH);
	//SSDL_RenderFillRect(paddle.xPos, paddle.yPos, PADDLEW, PADDLEH);		
}

int getScore()
{
	return currentScore++;
}

void generateColors(int colors[ROWS * COLS])
{
	for(int i = 0; i < ROWS * COLS; i++)
		colors[i] = rand() % 205;
}
SSDL_Color newColor(int r, int g, int b)
{

	SSDL_Color result
		= SSDL_CreateColor(r, g, b);

	return result;
}

void displayLives(int nLives)
{
	SSDL_SetRenderDrawColor(BLACK);
	SSDL_SetCursor(WINDOWW - ((BALLRADIUS * 2) * 8), 10);
	sout << "Lives: ";

	for (int i = 0; i < nLives; i++)
	{
		

		SSDL_SetRenderDrawColor(RED);
		SSDL_RenderFillCircle((i *(BALLRADIUS *3)) + WINDOWW - ((BALLRADIUS * 2) * 5), 15, BALLRADIUS);
		/*(WINDOWW - ((BALLRADIUS * 2) * 5))*/
		//WINDOWW - ((BALLRADIUS * 2) * 5)
	}
}