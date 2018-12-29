#include <iostream>
#include <math.h>
#include "SBDL.h"

using namespace std;

int main(int argc, char *argv[])
{
	const int windowWidth = 500;
	const int windowHeight = 500;
	SBDL::InitEngine("BallFollow", windowWidth, windowHeight);

	Texture blue = SBDL::loadTexture("assets/Blue.png");
	Texture red = SBDL::loadTexture("assets/Red.png");
	Texture play_button = SBDL::loadTexture("assets/Play.png");
	Texture font_texture, win_lose_texture;
	Sound *sound = SBDL::loadSound("assets/die.wav");
	Music *music = SBDL::loadMusic("assets/music.wav");
	Font *font = SBDL::loadFont("assets/times.ttf", 20);
	SBDL::playMusic(music, -1);

	int x = 10, y = 10, default_speed = 10, speed = default_speed, x_default = 10, y_deafult = 100;
	int xr_default = windowWidth - 10, yr_default = windowHeight - 10, xr = xr_default, yr = yr_default, default_enemy_speed = 5, enemy_speed = default_enemy_speed;
	bool lose = false;
	int i = 0;
	int interval = 1000, angle = 0;
	int score = 0;

	while (SBDL::isRunning()) {
		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		interval -= 30;
		if (lose) {
			SBDL::showTexture(blue, x, y);
			SBDL::showTexture(red, xr, yr);
			SBDL::freeTexture(win_lose_texture);
			win_lose_texture = SBDL::createFontTexture(font, "You Lose! Your Score: " + to_string(score), 0, 0, 0);
			SBDL::showTexture(win_lose_texture, (windowWidth / 2) - (win_lose_texture.width / 2), (windowHeight / 2) - (win_lose_texture.height / 2));
			SDL_Rect play_rect = { (windowWidth / 2) - (play_button.width / 2), (windowHeight / 2) + (play_button.height / 2) + 10 , play_button.width, play_button.height };
			SBDL::showTexture(play_button, play_rect);
			if (SBDL::mouseInRect(play_rect) && SBDL::Mouse.clicked()) {
				score = 0;
				lose = false;
				x = x_default;
				y = y_deafult;
				xr = xr_default;
				yr = yr_default;
			}
		}
		else {
			if (interval < 0) {
				score++;
				interval = 1000;
			}
			SBDL::freeTexture(font_texture);
			font_texture = SBDL::createFontTexture(font, "score: " + to_string(score), 0, 0, 0);
			SBDL::showTexture(font_texture, windowWidth - font_texture.width - 10, 10);

			enemy_speed = default_enemy_speed + score / 2;
			speed = default_speed + score / 4;

			if (SBDL::keyHeld(SDL_SCANCODE_RIGHT))
				x += speed;
			if (SBDL::keyHeld(SDL_SCANCODE_LEFT))
				x -= speed;
			if (SBDL::keyHeld(SDL_SCANCODE_DOWN))
				y += speed;
			if (SBDL::keyHeld(SDL_SCANCODE_UP))
				y -= speed;
			int vec_x = x - xr;
			int vec_y = y - yr;
			double size = sqrt(vec_x * vec_x + vec_y * vec_y);
			double x_change = vec_x / size * enemy_speed;
			double y_change = vec_y / size * enemy_speed;
			xr += x_change;
			yr += y_change;
			if (x > windowWidth)
				x = -blue.width;
			if (y > windowHeight)
				y = -blue.height;
			if (x + blue.width < 0)
				x = windowWidth;
			if (y + blue.height < 0)
				y = windowHeight;
			SBDL::showTexture(blue, x, y);
			SBDL::showTexture(red, xr, yr, angle);
			angle = (angle + 10) % 360;
			SDL_Rect red_rect = { xr, yr, red.width, red.height };
			SDL_Rect blue_rect = { x, y, blue.width, blue.height };
			if (SBDL::hasIntersectionRect(red_rect, blue_rect))
			{
				SBDL::playSound(sound, 1);
				lose = true;
			}
		}

		SBDL::updateRenderScreen();

		SBDL::delay(30);
	}

	return 0;
}
