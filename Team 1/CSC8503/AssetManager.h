#pragma once

#include "GameTechRenderer.h"

namespace NCL {
	namespace CSC8503 {
		class AssetManager {
		public:
			static void InitialiseAssets(GameTechRenderer* r);
			static void FreeAssets();

			static MeshGeometry* capsuleMesh;
			static MeshGeometry* cubeMesh;
			static MeshGeometry* sphereMesh;
			static MeshGeometry* basicLevelMesh;
			static MeshGeometry* charMesh;
			static MeshGeometry* enemyMesh;
			static MeshGeometry* bonusMesh;
			static MeshGeometry* complexMesh;
			static MeshGeometry* cylinderMesh;
			
			//static MeshGeometry* newcubeMesh;
			//static MeshGeometry* texcubeMesh;
			//static MeshGeometry* fishMesh;
			//static MeshGeometry* wallMesh;
			//static MeshGeometry* firstballMesh;
			//static MeshGeometry* secondballMesh;
			//static MeshGeometry* batMesh;

			

			static MeshMaterial* complexMat;
			static TextureBase* basicTex;
			static TextureBase* basicLevelTex;
			static TextureBase* menuTexture;
			static TextureBase* loadTexture;
			static TextureBase* endTexture;
			static TextureBase* loseTexture;
			static TextureBase* splatTexture;

			//static TextureBase* glassTexture;
			//static TextureBase* stoneTexture;
			//static TextureBase* waterTexture;
			//static TextureBase* ironTexture;
			//static TextureBase* newcubeTex;

			static TextureBase* animatedTexture[4];

			static ShaderBase* basicShader;
			static ShaderBase* wireframeShader;

			//static MeshMaterial* cubemat;
			static OGLComputeShader* paintShader;
			static OGLComputeShader* detectionShader;
		};
	}
}