#include "ConfigOptions.h"
#include "Assets.h"
#include <fstream>

using namespace NCL;
using namespace CSC8503;

float NCL::CSC8503::ConfigOptions::volume = NULL;
int NCL::CSC8503::ConfigOptions::screenX = NULL;
int NCL::CSC8503::ConfigOptions::screenY = NULL;
bool NCL::CSC8503::ConfigOptions::fullscreen = NULL;
float NCL::CSC8503::ConfigOptions::sensitivity = NULL;

bool ConfigOptions::LoadOptions(){
	std::ifstream inFile(Assets::DATADIR + "Config.txt");
	if (!inFile.good())
		return false;
	std::string line;
	std::getline(inFile, line);
	volume = stof(line);
	std::getline(inFile, line);
	screenX = stoi(line);
	std::getline(inFile, line);
	screenY = stoi(line);
	std::getline(inFile, line);
	fullscreen = stoi(line);
	std::getline(inFile, line);
	sensitivity = stof(line);

	return true;
}

void ConfigOptions::LoadDefault(){
	volume = 0.5f;
	screenX = 1280;
	screenY = 720;
	fullscreen = false;
	sensitivity = 1.0f;
}

void ConfigOptions::SaveOptions() {
	std::ofstream outFile(Assets::DATADIR + "Config.txt", std::ios::trunc);
	
	outFile << volume << std::endl;
	outFile << screenX << std::endl;
	outFile << screenY << std::endl;
	outFile << fullscreen << std::endl;
	outFile << sensitivity << std::endl;
}