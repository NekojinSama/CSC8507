#pragma once
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>

class SoundDevice
{
public:
	static SoundDevice* get();
	static void DeleteSoundDevice(SoundDevice* dead);
	
	ALCdevice* getDevice() { return device;  }
	ALCcontext* getContext() { return context; }

private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* device;
	ALCcontext* context;
};