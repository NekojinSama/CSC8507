#pragma once
#include <vector>
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>
#include <dr_lib/dr_wav.h>
#include <map>


class SoundBuffer
{
public:
	static SoundBuffer* get();

	ALuint addSoundEffect(const char* filename);
	bool removeSoundEffect(const char* filename);

	static void ClearSoundBuffer(SoundBuffer* soundValue);
	void deleteSoundBuffer();


private:
	SoundBuffer();
	~SoundBuffer();

	//std::vector<ALuint> SoundEffectBuffer;
	std::map<std::string, ALuint> sounds;
};