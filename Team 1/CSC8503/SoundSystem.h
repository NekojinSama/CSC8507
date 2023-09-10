#pragma once
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>
#include "GameObject.h"

#include <vector>
#include <algorithm>
#include "SoundDevice.h"
#include "SoundSource.h"


class SoundSource;

class SoundSystem {
public:
	static void Initialise(unsigned int channels = 32) {
		if (!instance) { instance = new SoundSystem(channels); }
	}
	static void Destroy() { delete instance; }
	inline static SoundSystem* GetSoundSystem() { return instance; }

	void AddSoundEmitter(SoundSource* s) { emitters.push_back(s); }
	void RemoveSoundEmitter(SoundSource * s);
	void Update(float msec);
	
	void SetMasterVolume(float value);
	OALSource* GetSource();

protected:
	~SoundSystem(void);
	void UpdateListener();
	void UpdateTemporaryEmitters(float msec);

	void SetListenerObject(NCL::CSC8503::GameObject* gameObject) { Listener = gameObject; }
	NCL::CSC8503::GameObject* GetListenerTransform() { return Listener; }

	
	void DetachSources(std::vector <SoundSource*>::iterator from, std::vector <SoundSource*>::iterator to);
	
	void AttachSources(std::vector <SoundSource*>::iterator from, std::vector <SoundSource*>::iterator to);
	
	void CullNodes();
	//OALSource* GetSource();

	SoundDevice* snd;
	ALCcontext* context;
	ALCdevice* deviceUse;
	float masterVolume;

	SoundSystem(unsigned int channels = 32);

	std::vector <OALSource*> sources;
	std::vector <SoundSource*> emitters;
	std::vector <SoundSource*> frameEmitters;

	NCL::CSC8503::GameObject* Listener;

	static SoundSystem* instance;
};