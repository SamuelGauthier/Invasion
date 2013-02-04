#include "invSound.h"

//----------------------------------
// Basic error checking function
// for FMOD
//----------------------------------
void ERRCHECK(FMOD_RESULT result)
{
	if(result != FMOD_OK)
	{
	}
}

//----------------------------------
// invSound
//----------------------------------
invSound::invSound()
{
	sound = 0;
}

invSound::~invSound()
{
	if(sound) ERRCHECK(FMOD_Sound_Release(sound));
}

void invSound::load(const char* filename, FMOD_SYSTEM* sys)
{
	printf("creating sound...\n");
	ERRCHECK(FMOD_System_CreateSound(sys, filename, FMOD_SOFTWARE, 0, &sound));
}

void invSound::play(FMOD_SYSTEM* sys)
{
	ERRCHECK(FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound, 0, NULL));
}

//----------------------------------
// invSoundSystem
//----------------------------------
invSoundSystem::invSoundSystem()
{
	ERRCHECK(FMOD_System_Create(&system));
}

invSoundSystem::~invSoundSystem()
{
	soundbank.clean();
	ERRCHECK(FMOD_System_Close(system));
	ERRCHECK(FMOD_System_Release(system));
}

int invSoundSystem::add(const char* filename)
{
	invSound* s = new invSound;
	s->load(filename, system);

	soundbank.addBack(s);

	return soundbank.count;
}

void invSoundSystem::play(int index)
{
	printf("playing sound...%d\n", index);
	soundbank.getIndex(index - 1)->play(system);
}
