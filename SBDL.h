/**
* SBDL: Sadegh & Borjian 's Direct media Layer! (not Shahid Beheshti's Direct media Layer)
*/
#pragma once

#include <string>
#include <utility>
#include <memory>

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
*
*/
struct Texture {
    /**
    * underneath texture which is loaded with SDL
    * don't use it directly in your code
    */
    SDL_Texture *underlineTexture;

    /**
     * position of this Texture
     * */
    SDL_Rect rect;
};

namespace SBDL {
    /**
    * don't import this namespace
    * this namespace used for handle underneath sdl functions for you
    */
    namespace Core {
        bool running = true;
        const std::uint8_t *keystates = nullptr;
        SDL_Event event;
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;

        Texture
        loadTextureUnderneath(const std::string &path, bool changeColor, std::uint8_t r, std::uint8_t g, std::uint8_t b,
                              int alpha = 100) {
            // Check existence of image
            SDL_Surface *pic = IMG_Load(path.c_str());
            if (pic == nullptr) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing Image file", path.c_str(), nullptr);
                exit(-1);
            }


            if (changeColor)
                SDL_SetColorKey(pic, SDL_TRUE, SDL_MapRGB(pic->format, r, g, b));
            if (alpha != 100)
                SDL_SetSurfaceAlphaMod(pic, (std::uint8_t) ((double) alpha / 100 * 255));

            Texture newTexture = {SDL_CreateTextureFromSurface(renderer, pic), {0, 0, pic->w, pic->h}};

            SDL_SetTextureBlendMode(newTexture.underlineTexture, SDL_BLENDMODE_BLEND);
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
        bool wheelUp;
        bool wheelDown;
    } Mouse;

    /**
    * used for checking state of program
    * @return state of SDL
    */
    bool isRunning() {
        return Core::running;
    }

    /**
    * initialize SDL and show a simple empty window for drawing texture on it
    * before start using SDL functions and types, first initialize engine
    * @param windowsTitle title of window
    * @param windowsWidth width of window
    * @param windowsHeight height of window
    */
    void InitEngine(const std::string &windowsTitle, int windowsWidth, int windowsHeight) {
        atexit(SDL_Quit); // set a SDL_Quit as exit function
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL initialization", "SDL initialize video engine error",
                                     nullptr);
            exit(1);
        }
        SDL_CreateWindowAndRenderer(windowsWidth, windowsHeight, SDL_WINDOW_SHOWN, &Core::window, &Core::renderer);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother
        SDL_RenderSetLogicalSize(Core::renderer, windowsWidth, windowsHeight);
        SDL_SetRenderDrawColor(Core::renderer, 255, 255, 255, 255);
        SDL_SetRenderDrawBlendMode(Core::renderer, SDL_BLENDMODE_BLEND);

        SDL_SetWindowTitle(Core::window, windowsTitle.c_str());
        // inilialize SDL_mixer, exit if fail
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL initialization", "SDL initialize audio engine error",
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
        Core::keystates = SDL_GetKeyboardState(nullptr);

        // reset event handler state for check it again
        Core::event = {};
        while (SDL_PollEvent(&Core::event)) { // loop until there is a new event for handling
            if (Core::event.type == SDL_MOUSEBUTTONDOWN || Core::event.type == SDL_MOUSEBUTTONUP) {
                // update state of Mouse structure if it was changed
                switch (Core::event.button.button) {
                    case 1:
                        Mouse.left = Core::event.type == SDL_MOUSEBUTTONDOWN ? true : false;
                        break;
                    case 2:
                        Mouse.middle = Core::event.type == SDL_MOUSEBUTTONDOWN ? true : false;
                        break;
                    case 3:
                        Mouse.right = Core::event.type == SDL_MOUSEBUTTONDOWN ? true : false;
                        break;
                    case 4:
                        Mouse.wheelUp = Core::event.type == SDL_MOUSEBUTTONDOWN ? true : false;
                        break;
                    case 5:
                        Mouse.wheelDown = Core::event.type == SDL_MOUSEBUTTONDOWN ? true : false;
                        break;
                }
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

    /*

    you can see all scanCode in sdl site
    */
    /**
    * indicate whether a key with specific scanCode is pressed or not
    * @param scanCode specific code for each keyboard button (https://wiki.libsdl.org/SDL_Scancode)
    * @return true if sepcific keyboard button pressed
    */
    bool key(SDL_Scancode scanCode) {
        return Core::keystates[scanCode] != 0;
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

    /*

    */
    /**
    * wait a few milliseconds before continue process of application
    * @param frameRate set the dalay (milisecond)
    */
    void delay(Uint32 frameRate) {
        SDL_Delay(frameRate);
    }

    /*

    */
    /**
    * load the font from a file
    * @param path path of the font file to load
    * @param size size of font
    * @return font which is loaded
    */
    Font *loadFont(const std::string &path, int size) {
        return TTF_OpenFont(path.c_str(), size);
    }

    /*
    get texture from a path of image
    */
    /**
    * load the texture from a file on disk
    * @param path path of the image file to load
    * @return texture which is loaded
    */
    Texture loadTexture(const std::string &path, int alpha = 100) {
        return Core::loadTextureUnderneath(path, false, 0, 0, 0, alpha);
    }

    /*
    get texture from a path of image and remove specific color from it
    */
    /**
    * load the texture from a file on disk and replace transparency of image with specific color
    * @param path path of the image file to load
    * @param r red color
    * @param g green color
    * @param b blue color
    * @param alpha transparency level
    * @return texture which is loaded
    */
    Texture loadTexture(const std::string &path, std::uint8_t r, std::uint8_t g, std::uint8_t b, int alpha = 100) {
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
    * load sound from a file in disk (use .ogg or .wav)
    * @param path path of the sound file to load
    * @return sound which is loaded
    */
    Sound *loadSound(const std::string &path) {
        return Mix_LoadWAV(path.c_str());
    }

    /**
    * load music from a file in disk (use .ogg or .wav)
    * @param path path of the music file to load
    * @return music which is loaded
    */
    Music *loadMusic(const std::string &path) {
        return Mix_LoadMUS(path.c_str());
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
        SDL_DestroyTexture(x.underlineTexture);
        x.underlineTexture = nullptr;
        x.rect = {0, 0, 0, 0};
    }


    /**
    * texture showed in render screen in position (x,y)
    * @param texture the source texture
    * @param x position of texture
    * @param y position of texture
    * @param angle an angle in degrees that indicates the rotation that will be applied to texture, rotating it in a clockwise direction around center of texture
    * @param flip flipping actions performed on the texture (SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL or SDL_FLIP_VERTICAL)
    * @param destRect optional rect if you want to draw texture on that
    */
    void showTexture(Texture texture, int x, int y, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE,
                     SDL_Rect destRect = {-1, -1, -1, -1}) {
        SDL_Rect rect;
        if (destRect == SDL_Rect{-1, -1, -1, -1})
            rect = {x, y, texture.rect.w, texture.rect.h};
        else
            rect = destRect;
        SDL_RenderCopyEx(Core::renderer, texture.underlineTexture, nullptr, &rect, angle, nullptr,
                         flip);
    }

    /**
    * texture showed in render screen in position (x,y)
    * @param texture the source texture
    * @param x position of texture
    * @param y position of texture
    * @param destRect optional rect if you want to draw texture on that
    */
    void showTexture(Texture texture, int x, int y, SDL_Rect destRect = {-1, -1, -1, -1}) {
        SDL_Rect rect;
        if (destRect == SDL_Rect{-1, -1, -1, -1})
            rect = {x, y, texture.rect.w, texture.rect.h};
        else
            rect = destRect;
        SDL_RenderCopy(Core::renderer, texture.underlineTexture, nullptr, &rect);
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
    Texture createFontTexture(Font *font, const std::string &text, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        SDL_Color color = {r, g, b};
        SDL_Surface *temp = TTF_RenderText_Solid(font, text.c_str(), color);

        Texture newTexture{SDL_CreateTextureFromSurface(Core::renderer, temp), {0, 0, temp->w, temp->h}};
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
}