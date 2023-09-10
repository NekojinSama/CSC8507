#include "SoundsManager.h"

map <std::string, Sounds*> SoundsManager::sounds;

void SoundsManager::AddSound(const char* filename) {
	if (!GetSound(filename)) {
		Sounds* s = new Sounds();
		s->LoadFromWAV(filename);
		sounds.insert(std::make_pair(filename, s));
	}
}

Sounds* SoundsManager::GetSound(std::string filename) {
	map <std::string, Sounds*>::iterator s = sounds.find(filename);
	return (s != sounds.end() ? s -> second : NULL);
}

void SoundsManager::DeleteSounds() {
	for (map <std::string, Sounds*>::iterator i = sounds.begin();
		i != sounds.end(); ++i) {
		delete i -> second;
	}
}
