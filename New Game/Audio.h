#pragma once
#include <SDL_mixer.h>
#include <vector>

using namespace std;

class Audio {
public:
	Audio();
	~Audio();
	void playSound(int sound);
	void playMusic(int music);
	enum sound {
		playershoot,
		playerhit,
		enemyshoot,
		enemyhit,
		explosion
	};
	enum music {
		menu,
		level1
	};
private:
	vector<Mix_Chunk*> sounds;
	vector<Mix_Music*> musics;
};

