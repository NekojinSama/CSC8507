#pragma once
#include "PS4RendererBase.h"
#include "PS4Shader.h"
#include "PS4Texture.h"
#include "PS4Mesh.h"
#include "GameWorld.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;
		class MainMenu;

		class PlayStation4Renderer : public PS4RendererBase	{
		public:
			PlayStation4Renderer(GameWorld& world, MainMenu* mainMenu);
			~PlayStation4Renderer();

			MeshGeometry*	LoadMesh(const string& name);
			TextureBase*	LoadTexture(const string& name);
			ShaderBase*		LoadShader(const string& vertex, const string& fragment);

		protected:
			virtual void OnWindowResize(int w, int h) override;

			void NewRenderLines();
			void NewRenderText();

			void RenderFrame(int type)	override;

			void RenderMainMenu();

			void RenderPauseMenu();

			void RenderGame();
			
			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void RenderSkybox();

			void LoadSkybox();

			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			vector<const RenderObject*> activeObjects;

			MainMenu* mainMenu;
		};

		class PlayStation4RendererFactory : RendererFactory{
		public:
			static RendererBase* GetNewRenderer(GameWorld& world, MainMenu* mainMenu) {
				return new PlayStation4Renderer(world, mainMenu);
			}
		};
	}
}

