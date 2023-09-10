#include "SoundSource.h"
#include <Vector3.h>

SoundSource::SoundSource(void)
{
	//alSourcef(p_Source, );
	Reset();
}

SoundSource::SoundSource(Sounds* s)
{
	Reset();
	SetSound(s);
}

SoundSource::~SoundSource()
{
	/*alDeleteSources(1, &p_Source);*/
	DetachSource();
}

void SoundSource::Reset()
{
	p_Priority = SOUNDPRIORTY_LOW;
	p_Volume = 1.0f;
	p_Radius = 500.0f;
	timeLeft = 0;
	currentSource = NULL;
	p_Sound = NULL;
}

void SoundSource::SetSound(Sounds* s)
{
	if (p_Sound != s) {
		p_Sound = s;
		DetachSource();
		if (s) {
			timeLeft = static_cast<drwav_uint64>(s->GetLength());
		}
	}
	else if(currentSource && currentSource->source){
		ALint sourceState;
		alGetSourcei(currentSource->source, AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING) {
			DetachSource();
			if (s) {
				timeLeft = s->GetLength();
			}
		}
	}
	
}

void SoundSource::AttachSource(OALSource* s)
{
	currentSource = s;

	if (!currentSource) {
		return;
	}

	currentSource->inUse = true;

	//alGenSources(1, &p_Source);
	alSourcef(currentSource->source, AL_MAX_DISTANCE, p_Radius);
	alSourcef(currentSource->source, AL_REFERENCE_DISTANCE, p_Radius * 0.2f);
	alSourcei(currentSource->source, AL_BUFFER, p_Sound->GetBuffer());
	//alSourcef(currentSource->source, AL_SEC_OFFSET, (p_Sound->GetLength() / 1000.0) - (timeLeft / 1000.0));
	alSource3f(currentSource->source, AL_VELOCITY, p_Velocity[0], p_Velocity[1], p_Velocity[2]);
	alSourcef(currentSource->source, AL_GAIN, p_Gain);
	alSourcef(currentSource->source, AL_PITCH, p_Pitch);
	alSourcePlay(currentSource->source);
}

void SoundSource::DetachSource()
{
	if (!currentSource) {
		return;
	}
	alSourcef(currentSource->source, AL_GAIN, 0.0f);
	alSourceStop(currentSource->source);
	alSourcei(currentSource->source, AL_BUFFER, 0);

	currentSource->inUse = false;
	currentSource = NULL;
}

bool SoundSource::CompareNodesByPriority(SoundSource* a, SoundSource* b)
{
	return (a -> p_Priority > b -> p_Priority) ? true : false;
}

void SoundSource::Update(float msec)
{
	timeLeft -= static_cast<drwav_uint64>(msec);

	while (p_LoopSound && timeLeft < 0.0f) {
		timeLeft += static_cast<drwav_uint64>(p_Sound->GetLength());
	}

	if (currentSource) {
		Vector3 pos;
		
		if (target) {
			pos = target->GetTransform().GetPosition();
		}
		else {
			pos = this->position;
		}
		
		alSourcefv(currentSource-> source, AL_POSITION, (float*)&pos);
		alSourcef(currentSource-> source, AL_GAIN, p_Volume);
		alSourcei(currentSource-> source, AL_LOOPING, p_LoopSound ? 1 : 0);
		alSourcef(currentSource-> source, AL_MAX_DISTANCE, p_Radius);
		alSourcef(currentSource-> source, AL_REFERENCE_DISTANCE, p_Radius * 0.2f);
	}
}

void SoundSource::RemoveSource() {
	alDeleteSources(1, &p_Source);
}

void SoundSource::Play(const ALuint bufferToPlay)
{
	if (bufferToPlay != p_Buffer) {
		p_Buffer = bufferToPlay;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	}
	ALint sourceState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &sourceState);
	if (sourceState == AL_PLAYING)
	{
		return;
	}
	alSourcePlay(p_Source);
}
