#ifndef INVSOUND_H
#define INVSOUND_H

#include <fmodex/fmod.h>
#include <stdio.h>
#include <stdlib.h>
#include "invList.h"

class invSound {
public:
	invSound ();
	~invSound ();

	void load(const char* filename, FMOD_SYSTEM* sys);
	void play(FMOD_SYSTEM* sys);
private:
	FMOD_SOUND* sound;

public:
	inline FMOD_SOUND* getHandle() { return sound; }
};

class invSoundSystem {
public:
	invSoundSystem ();
	~invSoundSystem ();

	int add(const char* filename);
	void play(int index);

private:
	invList<invSound> soundbank;
	FMOD_SYSTEM* system;
	int key, revision;
	unsigned int version;

};

#endif
