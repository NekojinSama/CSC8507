/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLTexture.h"
#include "OGLRenderer.h"

#include "TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;

OGLTexture::OGLTexture()
{
	glGenTextures(1, &texID);
}

OGLTexture::OGLTexture(GLuint texToOwn) {
	texID = texToOwn;
}

OGLTexture::~OGLTexture()
{
	glDeleteTextures(1, &texID);
}

TextureBase* OGLTexture::RGBATextureFromData(char* data, int width, int height, int channels) {
	OGLTexture* tex = new OGLTexture();

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel

	int sourceType = GL_RGB;

	switch (channels) {
		case 1: sourceType = GL_RED	; break;

		case 2: sourceType = GL_RG	; break;
		case 3: sourceType = GL_RGB	; break;
		case 4: sourceType = GL_RGBA; break;
		//default:
	}

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	tex->SetWidth(width);
	tex->SetHeight(height);
	return tex;
}

TextureBase* OGLTexture::RGBATextureFromFilename(const std::string&name) {
	char* texData	= nullptr;
	int width		= 0;
	int height		= 0;
	int channels	= 0;
	int flags		= 0;
	TextureLoader::LoadTexture(name, texData, width, height, channels, flags);  

	TextureBase* glTex = RGBATextureFromData(texData, width, height, channels);

	free(texData);

	return glTex;
}

TextureBase* OGLTexture::BindTextureImage(TextureBase* texture) { //Takes in a texture and creates an image the same size as input
	OGLTexture* mask = new OGLTexture();

	glBindTexture(GL_TEXTURE_2D, mask->GetObjectID());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture->GetWidth(), texture->GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);

	mask->SetHeight(texture->GetHeight());
	mask->SetWidth(texture->GetWidth());
	return mask;
}