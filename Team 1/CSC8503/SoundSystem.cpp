#include "SoundSystem.h"
#include "SoundsManager.h"
#include "ConfigOptions.h"
#include <iostream>

SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem(unsigned int channels) : masterVolume(NULL) {
	snd = SoundDevice::get();
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	
	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;
		
		alGenSources(1, &source);
		ALenum error = alGetError();
		
		if (error == AL_NO_ERROR) {
			sources.push_back(new OALSource(source));
		}
		else {
			break;
		}
	}
}

void SoundSystem::RemoveSoundEmitter(SoundSource* s)
{
}

void SoundSystem::Update(float msec)
{
	UpdateListener();
	
	for (std::vector <SoundSource*>::iterator i = emitters.begin(); i != emitters.end(); ++i) {
		frameEmitters.push_back((*i));
		(*i) -> Update(msec);
	}
	
	CullNodes();
	if (frameEmitters.size() > sources.size()) { 
		std::sort(frameEmitters.begin(), frameEmitters.end(), SoundSource::CompareNodesByPriority);
		
		DetachSources(frameEmitters.begin() + (sources.size() + 1), frameEmitters.end());
		AttachSources(frameEmitters.begin(), frameEmitters.begin() + sources.size());
	}
	else {
		AttachSources(frameEmitters.begin(), frameEmitters.end());
	}
	
	frameEmitters.clear();
}

void SoundSystem::SetMasterVolume(float value)
{
	value = std::max(0.0f, value);
	value = std::min(1.0f, value);
	masterVolume = value;
	alListenerf(AL_GAIN, masterVolume);
}

SoundSystem::~SoundSystem(void)
{
	/*alcMakeContextCurrent(NULL);*/
	for (std::vector <OALSource*>::iterator i = sources.begin(); i != sources.end(); ++i) {
		alDeleteSources(1, &(*i) -> source);
		delete (*i);
	}

	SoundsManager::DeleteSounds();
	SoundDevice::DeleteSoundDevice(snd);
}

void SoundSystem::UpdateListener()
{
	/*if (listener)*/ {
		Vector3 worldPos = Vector3();
		
		Vector3 dirup[2];
		// forward
			dirup[0].x = 0;
			dirup[0].y = 0;
			dirup[0].z = -1;
		// Up
			dirup[1].x = 0;
			dirup[1].y = 1;
			dirup[1].z = 0;

			alListenerfv(AL_POSITION, (float*)&worldPos);
			alListenerfv(AL_ORIENTATION, (float*)&dirup);
	}

}

void SoundSystem::UpdateTemporaryEmitters(float msec)
{
}

void SoundSystem::DetachSources(std::vector<SoundSource*>::iterator from, std::vector<SoundSource*>::iterator to)
{
	for (std::vector <SoundSource*>::iterator i = from; i != to; ++i) {
		(*i) -> DetachSource();
	}
}

void SoundSystem::AttachSources(std::vector<SoundSource*>::iterator from, std::vector<SoundSource*>::iterator to)
{
	for (std::vector<SoundSource*>::iterator i = from; i != to; ++i) {
		if (!(*i)->GetOALSource()) {
			(*i)->AttachSource(GetSource());
		}
	}
}

OALSource* SoundSystem::GetSource() {
	for (std::vector<OALSource*>::iterator i = sources.begin();
		i != sources.end(); ++i) {
		OALSource* s = *i;
		if (!s->inUse) {
			return s;
		}
	}
	return NULL;
}


void SoundSystem::CullNodes()
{
	for (std::vector < SoundSource* >::iterator i = frameEmitters.begin(); i != frameEmitters.end(); ) {
		SoundSource* e = (*i);
		float length = 0;
		
		if (e -> target) {
			length = (Listener->GetTransform().GetPosition() - e->target->GetTransform().GetPosition()).Length();
		}
		else {
			//length = (Listener->GetTransform().GetPosition() - e->target->GetTransform().GetPosition().Length());
		}
		
		if (length > e -> GetRadius() || !e -> GetSound() || e -> GetTimeLeft() < 0) {
			e -> DetachSource();
			i = frameEmitters.erase(i);
		}
		else {
			++i;
		}
	}
}

