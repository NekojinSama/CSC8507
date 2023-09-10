#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <../audioLib/AL/al.h>
#include <../audioLib/AL/alc.h>
#include <dr_lib/dr_wav.h>
#include "Sounds.h"
#include <map>

using std::map;
class SoundsManager {
public:
	static void AddSound(const char* filename);
	static Sounds* GetSound(std::string);
	
	static void DeleteSounds();

	
protected:
	SoundsManager(void);
	~SoundsManager(void);

	static map <std::string, Sounds*> sounds;
};
