#include "Sounds.h"

//std::map<std::string, Sounds*> Sounds::sounds;

Sounds::Sounds() {
	buffer = 0;
	bitRate = 0;
	freqRate = 0;
	length = 0;
	data = NULL;
}

Sounds::~Sounds(void) {
	delete data;
	alDeleteBuffers(1, &buffer);
}

ALenum Sounds::GetOALFormat() {
	if (GetBitRate() == 16) {
		return GetChannels() == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	}
	else if (GetBitRate() == 8) {
		return GetChannels() == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
	}
	return AL_FORMAT_MONO8;
}

float Sounds::GetLength() {
	return length;
}

void Sounds::LoadFromWAV(const char* filename) {
	ReadWavData monoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(filename, &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			drwav_free(pSampleData, nullptr);
		}
		if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			drwav_free(pSampleData, nullptr);
		}
		monoData.pcmData.resize(size_t(monoData.getTotalSamples()));
		std::memcpy(monoData.pcmData.data(), pSampleData, monoData.pcmData.size() * /*twobytes_in_s16*/2);
		drwav_free(pSampleData, nullptr);
	}
	ALuint monoSoundBuffer;
	alGenBuffers(1, &monoSoundBuffer);
	alBufferData(monoSoundBuffer, monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
		monoData.pcmData.data(), monoData.pcmData.size() * 2 /*two bytes per sample*/, monoData.sampleRate);

	bitRate = monoData.sampleRate;
	channels = monoData.channels;
	length = monoData.pcmData.size() * 2;
	buffer = monoSoundBuffer;
}
