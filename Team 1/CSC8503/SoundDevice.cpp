#include "SoundDevice.h"

SoundDevice* SoundDevice::get()
{
	static SoundDevice* snd_device = new SoundDevice();
	return snd_device;
}

void SoundDevice::DeleteSoundDevice(SoundDevice* dead)
{
	dead->~SoundDevice();
}

SoundDevice::SoundDevice()
{
	const ALCchar* defaultDeviceString = alcGetString(/*device*/nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	device = alcOpenDevice(defaultDeviceString);
	if (!device)
	{
		throw ("failed to get the default device for OpenAL");
	}
	context = alcCreateContext(device, /*attrlist*/ nullptr);
	if (!alcMakeContextCurrent(context))
	{
		throw ("failed to make the OpenAL context the current context");
	}
}

SoundDevice::~SoundDevice()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}