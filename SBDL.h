/**
* SBDL: Sadegh & Borjian Directmedia Layer!
*/
#pragma once

#include <string>

#if defined(_WIN32) || defined(_WIN64) // Windows

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

#elif defined(__linux__)  // linux

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#endif
#undef main

using Sound = Mix_Chunk;
using Music = Mix_Music;
using Font = TTF_Font;

/**
* Texture living on the graphics card that can be used for drawing.
*/
struct Texture {
	/**
	* underneath texture which is loaded with SDL
	* don't use it directly in your code
	*/
	SDL_Texture *underneathTexture = nullptr;

	/**
	* size of this Texture
	* */
	int width, height;
};

namespace SBDL {
	/**
	* don't import this namespace
	* this namespace used for handle underneath SDL functions for you
	*/
	namespace Core {
		/**
		* current state of SDL
		*/
		bool running = true;

		/**
		* SDL keyboard state array size
		*/
		int keystate_size = -1;

		/**
		* SDL current keyboard state
		*/
		const Uint8 *keystate = nullptr;

		/**
		* SDL last keyboard state
		*/
		Uint8 *old_keystate = nullptr;

		/**
		* SDL current event
		*/
		SDL_Event event;

		/**
		* SDL windows
		*/
		SDL_Window *window = nullptr;

		/**
		* SDL renderer
		*/
		SDL_Renderer *renderer = nullptr;

		/**
		 * create texture with given features
		 * @param path path of texture
		 * @param changeColor true if given color must be replaced with transparent color
		 * @param r red color
		 * @param g green color
		 * @param b blue color
		 * @param alpha transparency level
		 * @return texture which is created
		 */
		Texture loadTextureUnderneath(const std::string &path, bool changeColor, Uint8 r, Uint8 g, Uint8 b,
			Uint8 alpha = 255) {
			// Check existence of image
			SDL_Surface *pic = IMG_Load(path.c_str());
			if (pic == nullptr) {
				const std::string message = "Missing Image file: " + path;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBDL load image error", message.c_str(), nullptr);
				exit(1);
			}


			if (changeColor)
				SDL_SetColorKey(pic, SDL_TRUE, SDL_MapRGB(pic->format, r, g, b));
			if (alpha != 255)
				SDL_SetSurfaceAlphaMod(pic, alpha);

			Texture newTexture;
			newTexture.underneathTexture = SDL_CreateTextureFromSurface(renderer, pic);
			newTexture.width = pic->w;
			newTexture.height = pic->h;

			SDL_SetTextureBlendMode(newTexture.underneathTexture, SDL_BLENDMODE_BLEND);
			SDL_FreeSurface(pic);			

			return newTexture;
		}
	}

	/**
	* comparator of SDL_Rect
	*/
	bool operator==(const SDL_Rect &x, const SDL_Rect &y) {
		return x.x == y.x && x.y == y.y && x.h == y.h && x.w == y.w;
	}

	/**
	* a structure which give useful information about mouse state
	* update method must be call before using
	*/
	struct {
		int x;
		int y;
		bool left;
		bool right;
		bool middle;
		Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
		Uint8 clicks;
		Uint8 button; /**for SDL backward compatibility*/

		/**
		 * Check if Mouse clicked with given conditions.
		 * @param button button to check <SDL_BUTTON_LEFT,SDL_BUTTON_RIGHT,SDL_BUTTON_MIDDLE>
		 * @param clicks number of clicks
		 * @param state sate of mouse <SDL_PRESSED,SDL_RELEASED>
		 */
		bool clicked(Uint8 button = SDL_BUTTON_LEFT, Uint8 clicks = 1, Uint8 state = SDL_PRESSED) {
			return this->button == button && this->clicks == clicks && this->state == state;
		}
	} Mouse;

	/**
	* used for checking state of program
	* @return state of SDL
	*/
	bool isRunning() {
		return Core::running;
	}

	/**
	* stop SBDL and SBDL::isRunning will return false from now
	*/
	void stop() {
		Core::running = false;
	}

	/**
	* set the user-resizable state of a window
	*/
	void setWindowResizable(bool resizable) {
		SDL_SetWindowResizable(Core::window, resizable ? SDL_TRUE : SDL_FALSE);
	}

	/**
	* initialize SDL and show a simple empty window for drawing texture on it
	* before start using SDL functions and types, first initialize engine
	* @param windowsTitle title of window
	* @param windowsWidth width of window
	* @param windowsHeight height of window
	* @param r red color of default background
	* @param g green color of default background
	* @param b blue color of default background
	*/
	void InitEngine(const std::string &windowsTitle, int windowsWidth, int windowsHeight,
		Uint8 r = 255, Uint8 g = 255, Uint8 b = 255) {
		atexit(SDL_Quit); // set a SDL_Quit as exit function
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBDL initialization", "SBDL initialize video engine error",
				nullptr);
			exit(1);
		}

		SDL_CreateWindowAndRenderer(windowsWidth, windowsHeight, SDL_WINDOW_SHOWN, &Core::window, &Core::renderer);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother
		SDL_RenderSetLogicalSize(Core::renderer, windowsWidth, windowsHeight);
		SDL_SetRenderDrawColor(Core::renderer, r, g, b, 255);
		SDL_SetRenderDrawBlendMode(Core::renderer, SDL_BLENDMODE_BLEND);

		SDL_SetWindowTitle(Core::window, windowsTitle.c_str());
		// inilialize SDL_mixer, exit if fail
		if (SDL_Init(SDL_INIT_AUDIO) < 0) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBDL initialization", "SBDL initialize audio engine error",
				nullptr);
			exit(1);
		}

		// setup audio mode
		Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
		// setup text system
		TTF_Init();
	}

	/**
	* update state of keyboard buttons (release or push) and mouse
	* if ESCAPE pressed, application will stop
	* call this function in a loop after initialize engine for get updated state all times
	*/
	void updateEvents() {
		// update keyboard state
		if (Core::keystate_size == -1) {
			Core::keystate = SDL_GetKeyboardState(&Core::keystate_size);
			Core::old_keystate = new Uint8[Core::keystate_size];
			for (size_t i = 0; i < Core::keystate_size; i++)
				Core::old_keystate[i] = 0;
		}
		else {
			for (size_t i = 0; i < Core::keystate_size; i++)
				Core::old_keystate[i] = Core::keystate[i];
			Core::keystate = SDL_GetKeyboardState(&Core::keystate_size);
		}

		// reset event handler state for check it again
		Core::event = {};

		// returns true if there is an event in the queue, but will not remove it
		if (!SDL_PollEvent(nullptr)) {
			Mouse.left = Mouse.middle = Mouse.right = false;
			Mouse.button = 0;
			return;
		}
		while (SDL_PollEvent(&Core::event)) { // loop until there is a new event for handling
			if (Core::event.type == SDL_MOUSEBUTTONDOWN || Core::event.type == SDL_MOUSEBUTTONUP) {
				// update state of Mouse structure if it was changed
				switch (Core::event.button.button) {
				case 1:
					Mouse.left = true;
					Mouse.right = Mouse.middle = false;
					break;
				case 2:
					Mouse.middle = true;
					Mouse.right = Mouse.left = false;
					break;
				case 3:
					Mouse.right = true;
					Mouse.left = Mouse.middle = false;
					break;
				default:
					Mouse.left = Mouse.middle = Mouse.right = false;
				}

				Mouse.state = Core::event.button.state;
				Mouse.button = Core::event.button.button;
				Mouse.clicks = Core::event.button.clicks;
			}
			// update position of mouse if it was changed
			if (Core::event.type == SDL_MOUSEMOTION) {
				Mouse.x = Core::event.motion.x;
				Mouse.y = Core::event.motion.y;
			}
			// close app if ESCAPE pressed
			if (Core::event.type == SDL_QUIT || Core::event.key.keysym.sym == SDLK_ESCAPE) {
				Core::running = false;
			}
		}
	}

	/**
	* indicate whether a key with specific scanCode was pressed
	* @param scanCode specific code for each keyboard button (https://wiki.libsdl.org/SDL_Scancode)
	* @return true if specific keyboard button was pressed
	*/
	bool keyPressed(SDL_Scancode scanCode) {
		return !Core::old_keystate[scanCode] && Core::keystate[scanCode];
	}

	/**
	* indicate whether a key with specific scanCode was released
	* @param scanCode specific code for each keyboard button (https://wiki.libsdl.org/SDL_Scancode)
	* @return true if specific keyboard button was released
	*/
	bool keyReleased(SDL_Scancode scanCode) {
		return Core::old_keystate[scanCode] && !Core::keystate[scanCode];
	}

	/**
	* indicate whether a key with specific scanCode is hold
	* @param scanCode specific code for each keyboard button (https://wiki.libsdl.org/SDL_Scancode)
	* @return true if specific keyboard button is hold
	*/
	bool keyHeld(SDL_Scancode scanCode) {
		return Core::old_keystate[scanCode] && Core::keystate[scanCode];
	}

	/**
	* get Milliseconds since program was started.
	*/
	unsigned int getTime() {
		return SDL_GetTicks();
	}

	/**
	* clear the current rendering target
	*/
	void clearRenderScreen() {
		SDL_RenderClear(Core::renderer);
	}

	/**
	* update the screen and apply all changes
	*/
	void updateRenderScreen() {
		SDL_RenderPresent(Core::renderer);
	}

	/**
	* wait a few milliseconds before continue process of application
	* @param frameRate set the dalay (milisecond)
	*/
	void delay(Uint32 frameRate) {
		SDL_Delay(frameRate);
	}

	/**
	* load the font from a file
	* @param path path of the font file to load
	* @param size size of font
	* @return font which is loaded
	*/
	Font *loadFont(const std::string &path, int size) {
		return TTF_OpenFont(path.c_str(), size);
	}

	/**
	* load the texture from a file on disk
	* @param path path of the image file to load
	* @return texture which is loaded
	*/
	Texture loadTexture(const std::string &path, Uint8 alpha = 255) {
		return Core::loadTextureUnderneath(path, false, 0, 0, 0, alpha);
	}

	/**
	* load the texture from a file on disk and replace transparency of image with specific color
	* @param path path of the image file to load
	* @param r red color
	* @param g green color
	* @param b blue color
	* @param alpha transparency level
	* @return texture which is loaded
	*/
	Texture loadTexture(const std::string &path, Uint8 r, Uint8 g, Uint8 b, Uint8 alpha = 255) {
		return Core::loadTextureUnderneath(path, true, r, g, b, alpha);
	}

	/**
	* play sound
	* multiple sound can play concurrently
	* @param sound sound which is loaded before
	* @param count frequency of sound (-1 to play all time)
	* @see loadSound
	*/
	void playSound(Sound *sound, int count) {
		if (count != 0)
			Mix_PlayChannel(-1, sound, (count > 0) ? count - 1 : -1);
	}

	/**
	* play music
	* only one music file can play
	* @param music music which is loaded before
	* @param count frequency of msuic (-1 to play all time)
	* @see loadMusic
	*/
	void playMusic(Music *music, int count) {
		Mix_PlayMusic(music, count);
	}

	/**
	* load sound from a file in disk (use .wav)
	* @param path path of the sound file to load
	* @return sound which is loaded
	*/
	Sound *loadSound(const std::string &path) {
		Sound *sound;
		sound = Mix_LoadWAV(path.c_str());
		if (!sound) {
			const std::string message = "Unable to load: " + path;
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBDL load sound error", message.c_str(), nullptr);
			exit(1);
		}
		return sound;
	}

	/**
	* load music from a file in disk (use .ogg or .wav)
	* @param path path of the music file to load
	* @return music which is loaded
	*/
	Music *loadMusic(const std::string &path) {
		Music *music;
		music = Mix_LoadMUS(path.c_str());
		if (!music) {
			const std::string message = "Unable to load: " + path;
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBDL load music error", message.c_str(), nullptr);
			exit(1);
		}
		return music;
	}

	/**
	* stop music
	*/
	void stopMusic() {
		Mix_HaltMusic();
	}

	/**
	* stop all sounds
	*/
	void stopAllSounds() {
		Mix_HaltChannel(-1);
	}

	/**
	* free memory which is used for load sound from file
	* @param sound strcuture
	*/
	void freeSound(Sound *sound) {
		Mix_FreeChunk(sound);
	}

	/**
	* free memory which is used for load music from file
	* @param music strcuture
	*/
	void freeMusic(Music *music) {
		Mix_FreeMusic(music);
	}

	/**
	* free memory which is used for texture
	* After call this function, texture is not usable anymore and any using has undefined behavior
	* @param texture strcuture
	*/
	void freeTexture(Texture &x) {
		SDL_DestroyTexture(x.underneathTexture);
		x.underneathTexture = nullptr;
		x.width = 0;
		x.height = 0;
	}

	/**
	* texture showed in render screen in position destRect with angle and flip
	* @param texture the source texture
	* @param angle an angle in degrees that indicates the rotation that will be applied to texture, rotating it in a clockwise direction around center of texture
	* @param destRect  custom rect to draw texture
	* @param flip flipping actions performed on the texture (SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL or SDL_FLIP_VERTICAL)
	*/
	void showTexture(const Texture &texture, double angle, const SDL_Rect &destRect,
		SDL_RendererFlip flip = SDL_FLIP_NONE) {
		SDL_RenderCopyEx(Core::renderer, texture.underneathTexture, nullptr, &destRect, angle, nullptr,
			flip);
	}

	/**
	* texture showed in render screen in position texture.rect with angle and flip
	* @param texture the source texture
	* @param x position x
	* @param y position y
	* @param angle an angle in degrees that indicates the rotation that will be applied to texture, rotating it
	in a clockwise direction around center of texture
	* @param flip flipping actions performed on the texture (SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL or SDL_FLIP_VERTICAL)
	*/
	void showTexture(const Texture &texture, int x, int y, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = texture.width;
		rect.h = texture.height;
		showTexture(texture, angle, rect, flip);
	}

	/**
	* texture showed in render screen in position destRect
	* @param texture the source texture
	* @param destRect custom rect to draw texture
	*/
	void showTexture(const Texture &texture, const SDL_Rect &destRect) {
		SDL_RenderCopy(Core::renderer, texture.underneathTexture, nullptr, &destRect);
	}

	/**
	* texture showed in render screen in position texture.rect
	* @param texture the source texture
	* @param x position x
	* @param y position y
	*/
	void showTexture(const Texture &texture, int x, int y) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = texture.width;
		rect.h = texture.height;
		showTexture(texture, rect);
	}

	/**
	* create a texture from a font for a special string with specific color whcih can be drawed in render window
	* @param font font which is loaded
	* @param text text that convert to texture
	* @param r red color
	* @param g green color
	* @param b blue color
	* @return texture which created with that font
	*/
	Texture createFontTexture(Font *font, const std::string &text, Uint8 r, Uint8 g, Uint8 b) {
		SDL_Color color;
		color.r = r;
		color.g = g;
		color.b = b;
		SDL_Surface *temp = TTF_RenderText_Solid(font, text.c_str(), color);

		Texture newTexture;
		newTexture.underneathTexture = SDL_CreateTextureFromSurface(Core::renderer, temp);
		newTexture.width = temp->w;
		newTexture.height = temp->h;

		SDL_FreeSurface(temp);
		return newTexture;
	}

	/**
	* check intersection of two SDL_Rect
	* @param x first rectangle
	* @param y second rectangle
	* @return true if intersect each other
	*/
	bool hasIntersectionRect(const SDL_Rect &x, const SDL_Rect &y) {
		return SDL_HasIntersection(&x, &y) == SDL_TRUE;
	}

	/**
	* Draw rectangle on renderer screen.
	* @param rect rectangle position
	* @param r red
	* @param g green
	* @param b blue
	* @param alpha transparency
	*/
	void drawRectangle(const SDL_Rect &rect, Uint8 r, Uint8 g, Uint8 b, Uint8 alpha = 255) {
		Uint8 defaults[4];
		SDL_GetRenderDrawColor(Core::renderer, &defaults[0], &defaults[1], &defaults[2], &defaults[3]);
		SDL_SetRenderDrawColor(Core::renderer, r, g, b, alpha);
		SDL_RenderFillRect(Core::renderer, &rect);
		SDL_SetRenderDrawColor(Core::renderer, defaults[0], defaults[1], defaults[2], defaults[3]);
	}

	/**
	* Check if a point is inside a Rect
	* @param x
	* @param y
	* @param rect the Rectangle to check with
	* @return true if point is inside the rect
	*/
	bool pointInRect(int x, int y, const SDL_Rect &rect) {
		SDL_Point point;
		point.x = x;
		point.y = y;
		return SDL_PointInRect(&point, &rect) == SDL_TRUE;
	}

	/**
	* Check if mouse is inside a rect
	* @param rect the Rectangle to check with
	* @return true if mouse is inside the rect
	*/
	bool mouseInRect(const SDL_Rect &rect) {
		return pointInRect(Mouse.x, Mouse.y, rect);
	}
}
