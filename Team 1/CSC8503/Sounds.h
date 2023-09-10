#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>
#include <dr_lib/dr_wav.h>
#include <map>

struct ReadWavData
{
	unsigned int channels = 0;
	unsigned int sampleRate = 0;
	drwav_uint64 totalPCMFrameCount = 0;
	std::vector<uint16_t> pcmData;
	drwav_uint64 getTotalSamples() { return totalPCMFrameCount * channels; }
};

class Sounds
{
	friend class SoundsManager;
public:
	char* GetData() { return data; }
	int GetBitRate() { return bitRate; }
	float GetFrequency() { return freqRate; }
	int GetChannels() { return channels; }
	int GetSize() { return size; }
	ALuint GetBuffer() { return buffer; }
	
	ALenum GetOALFormat();
	float GetLength();

	
protected:
	Sounds();
	~Sounds(void);
	
	void LoadFromWAV(const char* filename);
	void LoadWAVChunkInfo(std::ifstream & file, std::string & name, unsigned int& size);
	
	char* data;
	float length;
	int bitRate;
	float freqRate;
	int size;
	int channels;
	
	ALuint buffer;
};