# SBDL (Sadegh & Borjian Directmedia Layer)
A wrapper around SDL2 which make use of SDL much simpler

## Motivation
For years in Shahid Beheshti University, ITP final project was a game written using SDL library.
Since the bare SDL library is too verbose and hard to use for first-year students in college, each year TAs create their own wrapper for SDL to ease the SDL for students.
This library is successor of `Hash<Written for ITP96Fall>` and `Genio2<Written for ITP95Fall>` with great experience which we learned from those libraries.

Mohammad Sadegh Dehghan & Amin Borjian wrote this library to fulfill all the needs of a first-term student for ITP97Fall course and all upcoming ITP courses in future.

## Basic Usage
1. Put `include` directories of `SDL2`,`SDL2_image`,`SDL2_ttf`,`SDL2_mixer` in your compiler's include directory.
2. Put `lib`  directories of `SDL2`,`SDL2_image`,`SDL2_ttf`,`SDL2_mixer` in your linker's path.
3. Put `SDL2Main.lib`,`SDL2.lib`,`SDL2_image.lib`,`SDL2_mixer.lib`,`SDL2_ttf.lib` in linker's dependencies.
4. Start Coding:
```C++
#include "SBDL.h"

using namespace std;

int main(int argc, char *argv[])
{
	const int window_width = 500;
	const int window_height = 500;
	SBDL::InitEngine("SBDL Test", window_width, window_height);

	const int FPS = 60; //frame per second
	const int delay = 1000 / FPS; //delay we need at each frame


	while (SBDL::isRunning()) {
		int startTime = SBDL::getTime();
		
		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		//Game logic code

		SBDL::updateRenderScreen();
		
		int elapsedTime = SBDL::getTime() - startTime;
		if (elapsedTime < delay)
			SBDL::delay(delay - elapsedTime);

	}
}

```
## Contribution
If you find any bugs,need a new feature,etc feel free to create an issue[https://github.com/MSDehghan/SBDL/issues]

Pull requests are also appreciated.

## License
This library is released under GPL v3
