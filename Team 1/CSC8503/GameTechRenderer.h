#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLComputeShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"
#include "MeshMaterial.h"

#include "GameWorld.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;
		class MainMenu;
		class HUD;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world, MainMenu* mainMenu);
			~GameTechRenderer();

			void Update(float dt) override;

			MeshGeometry*	LoadMesh(const string& name);
			TextureBase*	LoadTexture(const string& name);
			ShaderBase*		LoadShader(const string& vertex, const string& fragment);
			//MeshMaterial* LoadMaterial(const string& name);

			void SetHUD(HUD* _hud) {
				hud = _hud;
			}
			
			GLuint getHUDtex() { GLuint hud = ((OGLTexture*)hudTexture)->GetObjectID(); return hud; }

			void RenderBlindingSplat();

		protected:
			void RenderFullScreenQuadWithTexture(GLuint texture, OGLMesh* mesh);
			void RenderWireframes(Camera* camera);

			void NewRenderLines();
			void NewRenderText(size_t cameraNum);

			void RenderFrame(int type)	override;

			void RenderMainMenu();
			void RenderPauseMenu();

			void RenderWinScreen();
			void RenderLoseScreen();

			void RenderGame(Camera* camera, size_t i, size_t numCameras);

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void UpdateShaderMatrices(OGLShader* shader);
			void SetUpBuffers();
			void FillBuffers(int i);
			void DrawSSAO();
			void CalculateSampleKernels();
			void GenerateScreenTexture(GLuint& into, int texSlot, bool depth = false, bool largeFormat = false);

			inline float lerp(float a, float b, float f) {
				return a + f * (b - a);
			}
			
			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(Camera* camera);
			void RenderSkybox(Camera* camera);
			void SetViewPort(int camersNumber, int totalCameras);
			

			void LoadSkybox();

			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			void TimeGo(float dt);
			void TimeRun(float dt);


			vector<const RenderObject*> activeObjects;
			vector<const RenderObject*> activeWireObjects;

			OGLShader*  debugShader;
			OGLShader*  skyboxShader;
			OGLShader*	gBufferShader;
			OGLShader*	ssaoShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;

			OGLMesh* quad;
			OGLMesh* smallQuad;
			float frame;
			OGLShader* quadShader;
			TextureBase* menuTexture;
			TextureBase* loadTexture;
			TextureBase* hudTexture;
			TextureBase* splatTexture;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			//Gbuffer
			GLuint gPosition;
			GLuint gNormal;
			GLuint gBuffer;
			GLuint gAlbedo;

			GLuint gBufferFBO;
			GLuint bufferColourTex;
			GLuint gBufferNormalTex;
			GLuint gBufferDepthTex;
			GLuint gBufferWorldPosTex;

			//SSAO
			GLuint noiseTex;
			GLuint ssaoFBO;
			GLuint ssaoColourTex;
			vector<Vector3> ssaoKernel;
			vector<Vector3> ssaoNoise;
			Vector2 screenSize;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

			Matrix4 projMatrix;
			Matrix4 modelMatrix;
			Matrix4 viewMatrix;
			Matrix4 objectColour;

			float timeFlag;

			MainMenu* mainMenu;
			HUD* hud;
		};
	}
}

