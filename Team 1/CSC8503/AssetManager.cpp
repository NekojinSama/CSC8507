#include "AssetManager.h"

NCL::MeshGeometry* AssetManager::capsuleMesh = nullptr;
NCL::MeshGeometry* AssetManager::cubeMesh = nullptr;
NCL::MeshGeometry* AssetManager::sphereMesh = nullptr;
NCL::MeshGeometry* AssetManager::basicLevelMesh = nullptr;
NCL::MeshGeometry* AssetManager::charMesh = nullptr;
NCL::MeshGeometry* AssetManager::enemyMesh = nullptr;
NCL::MeshGeometry* AssetManager::bonusMesh = nullptr;
NCL::MeshGeometry* AssetManager::cylinderMesh = nullptr;
//NCL::MeshGeometry* AssetManager::newcubeMesh = nullptr;
//NCL::MeshMaterial* AssetManager::cubemat = nullptr;
//NCL::MeshGeometry* AssetManager::texcubeMesh = nullptr;
//NCL::MeshGeometry* AssetManager::fishMesh = nullptr;
//NCL::MeshGeometry* AssetManager::wallMesh = nullptr;
//NCL::MeshGeometry* AssetManager::firstballMesh = nullptr;
//NCL::MeshGeometry* AssetManager::secondballMesh = nullptr;
//NCL::MeshGeometry* AssetManager::batMesh = nullptr;
NCL::MeshGeometry* AssetManager::complexMesh = nullptr;

NCL::MeshMaterial* AssetManager::complexMat = nullptr;
NCL::TextureBase* AssetManager::basicTex = nullptr;
//NCL::TextureBase* AssetManager::newcubeTex = nullptr;
NCL::TextureBase* AssetManager::basicLevelTex = nullptr;
NCL::TextureBase* AssetManager::loadTexture = nullptr;
NCL::TextureBase* AssetManager::menuTexture = nullptr;
NCL::TextureBase* AssetManager::endTexture = nullptr;
NCL::TextureBase* AssetManager::loseTexture = nullptr;
NCL::TextureBase* AssetManager::splatTexture = nullptr;
//NCL::TextureBase* AssetManager::glassTexture = nullptr;
//NCL::TextureBase* AssetManager::stoneTexture = nullptr;
//NCL::TextureBase* AssetManager::waterTexture = nullptr;
//NCL::TextureBase* AssetManager::ironTexture = nullptr;


NCL::TextureBase* AssetManager::animatedTexture[4];

NCL::ShaderBase* AssetManager::basicShader = nullptr;
NCL::ShaderBase* AssetManager::wireframeShader = nullptr;

NCL::OGLComputeShader* AssetManager::paintShader = nullptr;
NCL::OGLComputeShader* AssetManager::detectionShader = nullptr;

void AssetManager::InitialiseAssets(GameTechRenderer* renderer) {
	cubeMesh = renderer->LoadMesh("cube.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");
	charMesh = renderer->LoadMesh("goat.msh");
	enemyMesh = renderer->LoadMesh("Keeper.msh");
	bonusMesh = renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	basicLevelMesh = renderer->LoadMesh("GameObject.msh");
	cylinderMesh = renderer->LoadMesh("Cylinder.msh");
	complexMesh = renderer->LoadMesh("NewCube.msh");
	//texcubeMesh = renderer->LoadMesh("texcube.msh");
	//fishMesh = renderer->LoadMesh("Atlantic_bluefin_tuna.msh");
	//wallMesh = renderer->LoadMesh("StoneWall_FBX1.msh");
	//firstballMesh = renderer->LoadMesh("abstract_sphere(75).msh");
	//secondballMesh = renderer->LoadMesh("webtrcc.msh");
	//batMesh = renderer->LoadMesh("Bat.msh");


	complexMat = new MeshMaterial("NewCube.mat");
	complexMat->LoadTextures();
	basicTex = renderer->LoadTexture("checkerboard.png");
	//newcubeTex = renderer->LoadTexture("Cube_ambient.jpg");
	loadTexture = renderer->LoadTexture("Load.png");
	menuTexture = renderer->LoadTexture("Menu.png");
	endTexture = renderer->LoadTexture("EndScreen.png");
	loseTexture = renderer->LoadTexture("Lost.png");
	splatTexture = renderer->LoadTexture("splat.png");
	//glassTexture = renderer->LoadTexture("glass.jpg");
	//stoneTexture = renderer->LoadTexture("stone.jpg");
	//waterTexture = renderer->LoadTexture("water.jpg");
	//ironTexture = renderer->LoadTexture("iron.jpg");


	animatedTexture[0] = renderer->LoadTexture("Animated0.png");
	animatedTexture[1] = renderer->LoadTexture("Animated1.png");
	animatedTexture[2] = renderer->LoadTexture("Animated2.png");
	animatedTexture[3] = renderer->LoadTexture("Animated3.png");
	
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");
	wireframeShader = renderer->LoadShader("wireframe.vert", "wireframe.frag");

	
	paintShader = new OGLComputeShader("compute.comp");
	detectionShader = new OGLComputeShader("detection.comp");
}

void AssetManager::FreeAssets() {
	delete capsuleMesh;
	delete cubeMesh;
	delete sphereMesh;
	delete basicLevelMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete cylinderMesh;
	delete complexMesh;

	delete complexMat;
	delete basicTex;
	delete basicLevelTex;
	delete loadTexture;
	delete menuTexture;
	delete endTexture;
	delete loseTexture;
	delete splatTexture;

	delete animatedTexture[0];
	delete animatedTexture[1];
	delete animatedTexture[2];
	delete animatedTexture[3];

	delete basicShader;
	delete wireframeShader;

	delete paintShader;
	delete detectionShader;
}