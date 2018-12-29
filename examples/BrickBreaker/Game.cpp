#include "SBDL.h"
#include <cstdlib>
#include <ctime>

using namespace std;

Texture stone;

struct Block
{
	SDL_Rect pos;
	int blockNumber = 1;
	bool isBreaked = false;
	bool isStone = false;
};

struct MovingObject
{
	Texture texture;
	SDL_Rect pos;
	int vx = 0;
	int vy = 0;
};

void load();
void init();
void createStone(Block& block);
void draw();
void update();
void checkBlockIntersect();

Texture blockTextures[6];
Block blocks[11][7];
MovingObject plate;
MovingObject ball;

const int FPS = 100;
const int delay = 1000 / FPS;

int main()
{
	SBDL::InitEngine("ITSS", 814, 600);

	load();
	while (SBDL::isRunning())
	{
		int startLoop = SBDL::getTime();
		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		draw();
		update();
		
		SBDL::updateRenderScreen();
		int endLoop = SBDL::getTime() - startLoop;
		if (endLoop < delay)
			SBDL::delay(delay - endLoop);
	}
	return 0;
}

void load()
{
	srand(time(NULL));
	blockTextures[0] = SBDL::loadTexture("assets/block0.png");
	blockTextures[1] = SBDL::loadTexture("assets/block1.png");
	blockTextures[2] = SBDL::loadTexture("assets/block2.png");
	blockTextures[3] = SBDL::loadTexture("assets/block3.png");
	blockTextures[4] = SBDL::loadTexture("assets/block4.png");
	blockTextures[5] = SBDL::loadTexture("assets/block5.png");
	plate.texture = SBDL::loadTexture("assets/plate.png");
	plate.pos = { (814 - 84) / 2,600 - 18,84,18 };
	stone = SBDL::loadTexture("assets/stone.png");
	ball.texture = SBDL::loadTexture("assets/ball.png");
	ball.pos = { (814 - 26) / 2,300,26,26 };
	ball.vx = rand() % 3 - 1;
	if (ball.vx == 0) ball.vx = 1;
	ball.vy = 1;
	init();
}

void init()
{
	for (int j = 0; j < 7; ++j)
	{
		for (int i = 0; i < 11; ++i)
		{
			blocks[i][j].blockNumber = rand() % 6;
			blocks[i][j].pos.x = i * 74;
			blocks[i][j].pos.y = j * 38;
			blocks[i][j].pos.w = 74;
			blocks[i][j].pos.h = 38;
		}
	}
	createStone(blocks[3][6]);
	createStone(blocks[9][6]);
	createStone(blocks[5][3]);
}

void createStone(Block& block)
{
	block.blockNumber = -1;
	block.isStone = true;
}

void draw()
{
	for (int j = 0; j < 7; ++j)
	{
		for (int i = 0; i < 11; ++i)
		{
			if (blocks[i][j].isBreaked) continue;
			if(blocks[i][j].isStone)
				SBDL::showTexture(stone, blocks[i][j].pos);
			else
				SBDL::showTexture(blockTextures[blocks[i][j].blockNumber], blocks[i][j].pos);
		}
	}
	SBDL::showTexture(plate.texture, plate.pos);
	SBDL::showTexture(ball.texture, ball.pos);
}

void update()
{
	ball.pos.x += ball.vx;
	ball.pos.y += ball.vy;
	if (ball.pos.x > 814 - 26 || ball.pos.x < 0) ball.vx *= -1;
	if (ball.pos.y < 0) ball.vy *= -1;

	if (SBDL::hasIntersectionRect(plate.pos, ball.pos) && ball.vy > 0)
	{
		ball.vy *= -1;
		ball.vx *= rand() % 2 == 0 ? -1 : 1;
	}

	if (ball.pos.y > 600)
		SBDL::stop();

	plate.pos.x += plate.vx;
	if (plate.pos.x > 814 - plate.pos.w)
		plate.pos.x = 814 - plate.pos.w;
	else if (plate.pos.x < 0)
		plate.pos.x = 0;

	if(SBDL::keyHeld(SDL_SCANCODE_RIGHT))
		plate.vx = 5;
	else if (SBDL::keyHeld(SDL_SCANCODE_LEFT))
		plate.vx = -5;
	else
		plate.vx = 0;

	checkBlockIntersect();
}

void checkBlockIntersect()
{
	for (int j = 0; j < 7; ++j)
	{
		for (int i = 0; i < 11; ++i)
		{
			Block* b = &blocks[i][j];
			if (!b->isBreaked && SBDL::hasIntersectionRect(b->pos, ball.pos))
			{
				if (!b->isStone)
					b->isBreaked = true;
				ball.vx *= -1;
				ball.vy *= -1;
			}
		}
	}
}
