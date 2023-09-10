#pragma once
#include <vector>
#include "Sounds.h"
#include "SoundBuffer.h"
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>
#include <dr_lib/dr_wav.h>
#include "GameObject.h"

enum SoundPriority {
	SOUNDPRIORTY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

struct OALSource 
{
	ALuint source;
	bool inUse;

	OALSource(ALuint src) {
		source = src;
		inUse = false;
	}
};


class SoundSource
{
public:
	SoundSource(void);
	SoundSource(Sounds* s);
	~SoundSource(void);

	void Reset();

	void SetSound(Sounds* s);
	inline Sounds* GetSound() { return p_Sound; }

	inline void SetPriority(SoundPriority p) { p_Priority = p; }
	inline SoundPriority GetPriority() { return p_Priority; }

	inline void SetRadius(float r) { p_Radius = r; }
	inline float GetRadius() { return p_Radius; }

	inline void SetVolume(float v) { p_Volume = v; }
	inline float GetVolume() { return p_Volume; }

	inline void SetLooping(bool value) { p_LoopSound = value; }
	inline bool GetLooping() { return p_LoopSound; }

	inline void SetPitch(float p) { p_Pitch = p; }
	inline float GetPitch() { return p_Pitch; }

	inline void SetGain(float g) { p_Gain = g; }
	inline float GetGain() { return p_Gain; }

	inline float GetTimeLeft() { return static_cast<float>(timeLeft); }

	inline OALSource* GetOALSource() { return currentSource; }
	void AttachSource(OALSource* s);
	void DetachSource();

	static bool CompareNodesByPriority(SoundSource* a, SoundSource* b);
	virtual void Update(float msec);

	void RemoveSource();

	void Play(const ALuint bufferToPlay);
	
	NCL::CSC8503::GameObject* target;

private:
	ALuint p_Source;
	Sounds* p_Sound;
	OALSource* currentSource;
	SoundPriority p_Priority;
	float p_Radius;
	float p_Volume;
	float p_Pitch = 1.0f;
	float p_Gain = 1.0f;
	float p_Velocity[3] = { 0,0,0 };
	bool p_LoopSound = false;
	ALuint p_Buffer = 0;
	drwav_uint64 timeLeft;
	Vector3 position;
};