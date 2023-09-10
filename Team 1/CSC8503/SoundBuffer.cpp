#include "SoundBuffer.h"

SoundBuffer* SoundBuffer::get()
{
	static SoundBuffer* sndbuf = new SoundBuffer();
	return sndbuf;
}

ALuint SoundBuffer::addSoundEffect(const char* filename)
{
	ReadWavData monoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(filename, &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			drwav_free(pSampleData, nullptr); //todo use raii to clean this up
			return -1;
		}
		if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			drwav_free(pSampleData, nullptr);
			return -1;
		}
		monoData.pcmData.resize(size_t(monoData.getTotalSamples()));
		std::memcpy(monoData.pcmData.data(), pSampleData, monoData.pcmData.size() * /*twobytes_in_s16*/2);
		drwav_free(pSampleData, nullptr);
	}
	ALuint monoSoundBuffer;
	alGenBuffers(1, &monoSoundBuffer);
	alBufferData(monoSoundBuffer, monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, 
		monoData.pcmData.data(), monoData.pcmData.size() * 2 /*two bytes per sample*/, monoData.sampleRate);
	
	sounds.insert(std::make_pair(filename, monoSoundBuffer));
	//SoundEffectBuffer.push_back(monoSoundBuffer);
	return monoSoundBuffer;
}

bool SoundBuffer::removeSoundEffect(const char* filename)
{
	auto it = sounds.begin();
	while (it != sounds.end()) {
		if (it->first == filename)
		{
			alDeleteBuffers(1, &(*it).second);
			it = sounds.erase(it);
			return true;
		}
		else {
			++it;
		}
	}
	return false;
}

void SoundBuffer::ClearSoundBuffer(SoundBuffer* soundValue)
{
	delete soundValue;
}

void SoundBuffer::deleteSoundBuffer()
{
	auto it = sounds.begin();
	for (it; it != sounds.end(); it++) {
		alDeleteBuffers(1, &(*it).second);
	}
	sounds.clear();
}

SoundBuffer::SoundBuffer() {
	sounds.clear();
}

SoundBuffer::~SoundBuffer()
{
	deleteSoundBuffer();
}
