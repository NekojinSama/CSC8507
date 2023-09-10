#include "RenderObject.h"
#include "MeshGeometry.h"
#include "../NCLCoreClasses/TextureLoader.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, bool hasMask, MeshMaterial* mat) {
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;

	char* texData = nullptr;
	if (hasMask) {
		if (mat) {
			TextureBase* texture = dynamic_cast<TextureBase*>(mat->GetMaterialForLayer(0)->GetEntry("Diffuse"));
			material = mat;
			this->paintMask = TextureLoader::BindAPITextureImage(texture);
			isComplex = true;
		}
		else {
			this->paintMask = TextureLoader::BindAPITextureImage(tex);
		}
	}
	else {
		this->paintMask = nullptr;
	}
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::~RenderObject() {

}