/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <map>
#include <functional>
#include <string>

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}

	typedef std::function<bool(const std::string& filename, char*& outData, int& width, int &height, int &channels, int&flags)> TextureLoadFunction;

	typedef std::function<Rendering::TextureBase* (const std::string& filename)> APILoadFunction;
	typedef std::function<Rendering::TextureBase* (char* data, int width, int height, int channels)> APILoadDataFunction;
	typedef std::function<Rendering::TextureBase* (Rendering::TextureBase* texture)> APIBindImageFunction;

	class TextureLoader	{
	public:
		static bool LoadTexture(const std::string& filename, char*& outData, int& width, int &height, int &channels, int&flags);

		static void RegisterTextureLoadFunction(TextureLoadFunction f, const std::string&fileExtension);
		
		static void RegisterAPILoadFunction(APILoadFunction f);
		static void RegisterAPILoadDataFunction(APILoadDataFunction f);
		static void RegisterAPIBindImageFunction(APIBindImageFunction f);

		static Rendering::TextureBase* LoadAPITexture(const std::string&filename);
		static Rendering::TextureBase* LoadAPIDataTexture(char* data, int width, int height, int channels);
		static Rendering::TextureBase* BindAPITextureImage(Rendering::TextureBase* texture);

	protected:

		static std::string GetFileExtension(const std::string& fileExtension);

		static std::map<std::string, TextureLoadFunction> fileHandlers;

		static APILoadFunction apiFunction;
		static APILoadDataFunction apiDataFunction; 
		static APIBindImageFunction apiImageFunction; // Creates image units for compute shader
	};
}

