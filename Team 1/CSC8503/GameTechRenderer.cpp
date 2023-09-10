#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "Assets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <Win32Window.h>
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

#include "MainMenu.h"
#include "AssetManager.h"
#include "HUD.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world, MainMenu* menu) : OGLRenderer(*Window::GetWindow()), gameWorld(world), frame(0) {
	glEnable(GL_DEPTH_TEST);
	screenSize = Vector2(windowWidth, windowHeight);

	debugShader  = new OGLShader("debug.vert", "debug.frag");
	shadowShader = new OGLShader("shadow.vert", "shadow.frag");
	gBufferShader = new OGLShader("gBuffer.vert", "gBuffer.frag");
	ssaoShader = new OGLShader("ssao.vert", "ssao.frag");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(0.0f, 60.0f, 0.0f);

	//Skybox!
	skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();

	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);

	CalculateSampleKernels();
	SetUpBuffers();

	mainMenu = menu;
	hud = nullptr;

	quadShader = new OGLShader("quad.vert", "quad.frag");
	quad = new OGLMesh();
	quad = quad->GenerateQuadWithIndices();
	menuTexture = LoadTexture("Menu.png");
	loadTexture = LoadTexture("Load.png");
	hudTexture = LoadTexture("sprayIcon.png");
	splatTexture = LoadTexture("splat.png");
	smallQuad = new OGLMesh();
	smallQuad = smallQuad->GenerateSmallQuadWithIndices();

	IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		
		ImFont* MenuFont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "/Kanit/Kanit-Regular.ttf").c_str(), 60.0f);
		//ImFont* HUDFont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "PressStart2P-Regular.ttf").c_str(), 30.0f);
		ImGui::StyleColorsDark();
		
		Win32Code::Win32Window* realWindow = (Win32Code::Win32Window*)&hostWindow;
		
		ImGui_ImplWin32_Init(realWindow->GetHandle());
		ImGui_ImplOpenGL3_Init("#version 330");
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete quad;
}

void GameTechRenderer::Update(float dt) {
	frame = (frame + dt) < 4 ? frame + dt : 0.0f;

	if (screenSize.x != windowWidth) {
		screenSize = Vector2(windowWidth, windowHeight);
		SetUpBuffers();
	}
	timeFlag += dt;
}

void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6]	= { 0 };
	int height[6]	= { 0 };
	int channels[6] = { 0 };
	int flags[6]	= { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame(int type) {
	std::vector<Camera*> cameras = gameWorld.GetAllCameras();
	size_t numCameras = cameras.size();
	for (size_t i = 0; i < numCameras; i++) {
		glEnable(GL_CULL_FACE);
		glClearColor(1, 1, 1, 1);
		SetViewPort(0, 1);
		switch (type) {
		case 0:
			RenderGame(cameras.at(i), i, numCameras);
			if (hud != nullptr) { hud->GameHUD(this); hud->WeaponHUD(this, nullptr); hud->setCameraCount(numCameras); hud->CoopGameWeaponHUD(this); };
			break;
		case 1:
			i = numCameras;
			RenderMainMenu();
			break;
		case 2:
			i = numCameras;
			RenderPauseMenu();
			break;
		case 3:	// Load screen
			i = numCameras;
			RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::loadTexture)->GetObjectID(), quad);
			RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::animatedTexture[(int)frame])->GetObjectID(), smallQuad);
			break;
		case 4: // Win screen
			RenderWinScreen();
			break;
		case 5: // Lose screen
			RenderLoseScreen();
			break;
		case 6:
			RenderGame(cameras.at(i), i, numCameras);
			RenderWireframes(cameras.at(i));
			if (hud != nullptr) hud->DebugHUD(this);
			break;
		}
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		NewRenderLines();
		NewRenderText(i);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();
	activeWireObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				const RenderObject* w = o->GetWireframeObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
				if (w) {
					activeWireObjects.emplace_back(w);
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		Transform t = (*i).GetTransform()->GetWorldTransform();
		Matrix4 modelMatrix = t.GetMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	/*glViewport(0, (windowHeight / cameras)* cameraNum, windowWidth, windowHeight/ cameras);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);*/
}

void GameTechRenderer::SetViewPort(int cameraNum, int totalCameras) {
	glViewport(0, (windowHeight / totalCameras) * cameraNum, windowWidth, windowHeight / totalCameras);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox(Camera* camera) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)windowWidth / (float)windowHeight;
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = camera->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderFullScreenQuadWithTexture(GLuint texture, OGLMesh* mesh) {
	glDepthMask(false);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	BindShader(quadShader);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(quadShader->GetProgramID(), "mainTex"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	BindMesh(mesh);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDepthMask(true);
}

void GameTechRenderer::RenderGame(Camera* c, size_t i, size_t numCameras) {
	BuildObjectList();
	SortObjectList();

	FillBuffers(i);
	DrawSSAO();

	RenderShadowMap();
	SetViewPort(i, numCameras);
	RenderSkybox(c);
	RenderCamera(c);
}

void GameTechRenderer::RenderMainMenu() {
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::menuTexture)->GetObjectID(), quad);
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::animatedTexture[(int)frame])->GetObjectID(), smallQuad);
	mainMenu->UpdateMainMenu();
}

void GameTechRenderer::RenderPauseMenu() {
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::menuTexture)->GetObjectID(), quad);
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::animatedTexture[(int)frame])->GetObjectID(), smallQuad);
	mainMenu->UpdatePauseMenu();
}

void GameTechRenderer::RenderWinScreen() {
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::endTexture)->GetObjectID(), quad);
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::animatedTexture[(int)frame])->GetObjectID(), smallQuad);
	mainMenu->UpdateEndScreen();
}

void GameTechRenderer::RenderLoseScreen() {
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::loseTexture)->GetObjectID(), quad);
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::animatedTexture[(int)frame])->GetObjectID(), smallQuad);
	mainMenu->UpdateEndScreen();
}

void GameTechRenderer::RenderCamera(Camera* camera) {

	float screenAspect = (float)windowWidth / (float)windowHeight;
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = camera->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;

	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;

	int pixelSizeLocation = 0;
	int ssaoTexLocation = 0;

	int lightPosLocation	= 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int hasMaskLocation		= 0;
	int paintCentreLocation	= 0;

	int cameraLocation = 0;

	int hasFlag = 0;
	int usecolour = 0;
	float timeOfFlag = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		MeshMaterial* mat = (*i).GetMeshMaterial();
		if ((*i).isComplex == true) {
			//int texUnit = 4;

			//glActiveTexture(GL_TEXTURE0 + texUnit);
			//const std::string& uniform = "mainTex";
			//GLuint slot = glGetUniformLocation(shader->GetProgramID(), uniform.c_str());

			//if (slot < 0) {
			//	return;
			//}

			//GLint texID = 0;
			//const OGLTexture* tex = dynamic_cast<const OGLTexture*>(mat->GetMaterialForLayer(0)->GetEntry("Diffuse"));

			//if (tex) {
			//	texID = tex->GetObjectID();
			//}

			//glBindTexture(GL_TEXTURE_2D, texID);
			//glUniform1i(slot, texUnit);
			BindTextureToShader(dynamic_cast<const OGLTexture*>(mat->GetMaterialForLayer(0)->GetEntry("Diffuse")), "mainTex", 0);
		}
		else {
			BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);
		}

		BindTextureToShader((OGLTexture*)(*i).GetPaintMaskTexture(), "paintMask", 3);

		glUniform2f(glGetUniformLocation(shader->GetProgramID(), "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
		glUniform1f(glGetUniformLocation(shader->GetProgramID(), "toggleAO"), toggleAO);
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "ssaoTex"), 9);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, ssaoColourTex);

		if (activeShader != shader) {
			projLocation	= glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation	= glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			lightPosLocation	= glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

			hasMaskLocation = glGetUniformLocation(shader->GetProgramID(), "hasMask");
			paintCentreLocation = glGetUniformLocation(shader->GetProgramID(), "paintCentre");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");

			hasFlag = glGetUniformLocation(shader->GetProgramID(), "hasFlag");

			timeOfFlag = glGetUniformLocation(shader->GetProgramID(), "time");
			
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "time"), timeFlag);


			//usecolour = glGetUniformLocation(shader->GetProgramID(), "usecolor");

			Vector3 camPos = gameWorld.GetMainCamera()->GetPosition();
			glUniform3fv(cameraLocation, 1, camPos.array);

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(lightPosLocation	, 1, (float*)&lightPosition);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation , lightRadius);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

			activeShader = shader;
		}

		modelMatrix = (*i).GetTransform()->GetWorldTransform().GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			
		
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		Vector4 colour = i->GetColour();
		glUniform4fv(colourLocation, 1, colour.array);

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1:0);

		glUniform1i(hasMaskLocation, (OGLTexture*)(*i).GetPaintMaskTexture() ? 1 : 0);

		
		Vector3 paintCentre = (*i).GetPaintCentre();
		glUniform3fv(paintCentreLocation, 1, (float*)&paintCentre);

		glUniform1i(hasFlag, !(*i).GetShader()?1:0);


	    

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
}

void GameTechRenderer::RenderWireframes(Camera* camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float screenAspect = (float)windowWidth / (float)windowHeight;
	viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	BindShader((OGLShader*)AssetManager::wireframeShader);
	UpdateShaderMatrices((OGLShader*)AssetManager::wireframeShader);

	for (const auto& i : activeWireObjects) {
		if (i->GetMesh()) {
			modelMatrix = (*i).GetTransform()->GetWorldTransform().GetMatrix();
			UpdateShaderMatrices((OGLShader*)AssetManager::wireframeShader);

			BindMesh((*i).GetMesh());
			int layerCount = (*i).GetMesh()->GetSubMeshCount();
			for (int i = 0; i < layerCount; ++i) {
				DrawBoundMesh(i);
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	OGLShader* activeShader = nullptr;

	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto& i : activeWireObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		if (activeShader != shader) {
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			activeShader = shader;
		}

		modelMatrix = (*i).GetTransform()->GetWorldTransform().GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

		GeometryPrimitive primType = i->GetMesh()->GetPrimitiveType();
		i->GetMesh()->SetPrimitiveType(GeometryPrimitive::Lines);

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}

		i->GetMesh()->SetPrimitiveType(primType);
	}
}

void GameTechRenderer::RenderBlindingSplat() {
	RenderFullScreenQuadWithTexture(((OGLTexture*)AssetManager::splatTexture)->GetObjectID(), quad);
}

MeshGeometry* GameTechRenderer::LoadMesh(const string& name) {
	OGLMesh* mesh = new OGLMesh(name);
	mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	mesh->UploadToGPU();
	return mesh;
}

void GameTechRenderer::NewRenderLines() {
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	if (lines.empty()) {
		return;
	}
	float screenAspect = (float)windowWidth / (float)windowHeight;
	viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	
	Matrix4 viewProj  = projMatrix * viewMatrix;

	BindShader(debugShader);
	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 0);

	glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

	debugLineData.clear();

	int frameLineCount = lines.size() * 2;

	SetDebugLineBufferSizes(frameLineCount);

	glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameLineCount * sizeof(Debug::DebugLineEntry), lines.data());
	

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, frameLineCount);
	glBindVertexArray(0);
}

void GameTechRenderer::NewRenderText(size_t cameraNum) {
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();

	if (strings.empty()) {
		return;
	}

	int frameVertCount = 0;
	for (const auto& s : strings) {
		if(s.screen == cameraNum || s.screen == -1)
			frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
	}
	SetDebugStringBufferSizes(frameVertCount);

	BindShader(debugShader);

	OGLTexture* t = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

	if (t) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);	
		BindTextureToShader(t, "mainTex", 0);
	}
	Matrix4 proj = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);

	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 1);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	for (const auto& s : strings) {
		if (s.screen == cameraNum || s.screen == -1) {
			Vector2 size = Vector2(20.0f, 40.0f);
			Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
		}
	}


	glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
	glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
	glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

	glBindVertexArray(textVAO);
	glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
	glBindVertexArray(0);
}

TextureBase* GameTechRenderer::LoadTexture(const string& name) {
	return TextureLoader::LoadAPITexture(name);
}

ShaderBase* GameTechRenderer::LoadShader(const string& vertex, const string& fragment) {
	return new OGLShader(vertex, fragment);
}



void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
	if (newVertCount > textCount) {
		textCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

		debugTextPos.reserve(textCount);
		debugTextColours.reserve(textCount);
		debugTextUVs.reserve(textCount);

		glBindVertexArray(textVAO);

		glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

		glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
		glVertexAttribBinding(1, 1);
		glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

		glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
		glVertexAttribBinding(2, 2);
		glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
	if (newVertCount > lineCount) {
		lineCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
		glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

		debugLineData.reserve(lineCount);

		glBindVertexArray(lineVAO);

		int realStride = sizeof(Debug::DebugLineEntry) / 2;

		glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, lineVertVBO, 0, realStride);

		glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
		glVertexAttribBinding(1, 0);
		glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}

void GameTechRenderer::TimeGo(float dt)
{
	timeFlag += dt;
}

void GameTechRenderer::TimeRun(float dt)
{
	TimeGo(dt);//?????????
}

void GameTechRenderer::SetUpBuffers() {
	glGenFramebuffers(1, &gBufferFBO);

	GLenum buffers[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	int texSlot = 4;

	// Generate our scene depth texture ...
	GenerateScreenTexture(gBufferDepthTex, texSlot, true);
	GenerateScreenTexture(bufferColourTex, ++texSlot);
	GenerateScreenTexture(gBufferNormalTex, ++texSlot);
	GenerateScreenTexture(gBufferWorldPosTex, ++texSlot, false, true);

	//And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferWorldPosTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBufferDepthTex, 0);
	glDrawBuffers(3, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glGenFramebuffers(1, &ssaoFBO);
	GLenum buf = { GL_COLOR_ATTACHMENT0 };

	glGenTextures(1, &noiseTex);
	glActiveTexture(GL_TEXTURE0 + ++texSlot);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GenerateScreenTexture(ssaoColourTex, ++texSlot);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColourTex, 0);
	glDrawBuffer(buf);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
}

void GameTechRenderer::GenerateScreenTexture(GLuint& into, int texSlot, bool depth, bool largeFormat) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;
	format = largeFormat ? GL_RGBA32F : format;

	glTexImage2D(GL_TEXTURE_2D, 0, format, windowWidth, windowHeight, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameTechRenderer::FillBuffers(int camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float screenAspect = (float)windowWidth / (float)windowHeight;
	viewMatrix = gameWorld.GetCamera(camera)->BuildViewMatrix();
	projMatrix = gameWorld.GetCamera(camera)->BuildProjectionMatrix(screenAspect);

	BindShader(gBufferShader);
	UpdateShaderMatrices(gBufferShader);


	for (const auto& i : activeObjects) {
		if (i->GetMesh()) {
			modelMatrix = (*i).GetTransform()->GetWorldTransform().GetMatrix();
			UpdateShaderMatrices(gBufferShader);

			BindMesh((*i).GetMesh());
			int layerCount = (*i).GetMesh()->GetSubMeshCount();
			for (int i = 0; i < layerCount; ++i) {
				DrawBoundMesh(i);
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::DrawSSAO() {
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	BindShader(ssaoShader);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTex);

	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "texNoise"), 0);
	glUniform3fv(glGetUniformLocation(ssaoShader->GetProgramID(), "samples"), 64, (float*)ssaoKernel.data());
	glUniform2fv(glGetUniformLocation(ssaoShader->GetProgramID(), "screenSize"), 1, (float*)&screenSize);
	glUniform2f(glGetUniformLocation(ssaoShader->GetProgramID(), "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(ssaoShader->GetProgramID(), "inverseProjView"), 1, false, (float*)&invViewProj);
	UpdateShaderMatrices(ssaoShader);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "depthTex"), 1);
	glBindTexture(GL_TEXTURE_2D, gBufferDepthTex);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "normTex"), 2);
	glBindTexture(GL_TEXTURE_2D, gBufferNormalTex);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(ssaoShader->GetProgramID(), "worldPosTex"), 3);
	glBindTexture(GL_TEXTURE_2D, gBufferWorldPosTex);

	BindMesh(quad);
	DrawBoundMesh();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::UpdateShaderMatrices(OGLShader* shader) {
	if (shader) {
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "modelMatrix"), 1, false, (float*)&modelMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "viewMatrix"), 1, false, (float*)&viewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projMatrix"), 1, false, (float*)&projMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "objectColour"), 1, false, (float*)&objectColour);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "shadowMatrix"), 1, false, (float*)&shadowMatrix);
	}
}

void GameTechRenderer::CalculateSampleKernels() {
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	ssaoKernel.clear();
	ssaoNoise.clear();

	for (unsigned int i = 0; i < 64; ++i) {
		Vector3 sample = Vector3(randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator));

		sample.Normalise();
		sample = sample * randomFloats(generator);

		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample = sample * scale;
		ssaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < 16; i++) {
		Vector3 noise = Vector3(randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}
}