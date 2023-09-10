#pragma once
#include "PS4Mesh.h"
#include "PS4Shader.h"
#include "PS4Texture.h"
#include "PS4Frame.h"
#include "RendererBase.h"

namespace NCL {
	namespace Rendering {

		struct PS4ScreenBuffer {
			sce::Gnm::RenderTarget colourTarget;
			sce::Gnm::DepthRenderTarget depthTarget;
		};

		enum MemoryLocation {
			GARLIC,
			ONION,
			MEMORYMAX
		};

		class PS4RendererBase : public RendererBase, public PS4MemoryAware {
		public:
			PS4RendererBase(Window& w);
			~PS4RendererBase();

			void Update(float dt) override;

		protected:
			void SwapScreenBuffer();
			void SwapCommandBuffer();

			void BeginFrame()	override;
			void RenderFrame(int type)	override;
			void EndFrame()		override;
			void SwapBuffers() override;

		private:
			void InitialiseMemoryAllocators();
			void InitialiseVideoSystem();
			void InitialiseGCMRendering();
			void InitialiseExternalPointers();
			void DestroyMemoryAllocators();
			void DestroyVideoSystem();
			void DestroyGCMRendering();
			void SetRenderBuffer(PS4ScreenBuffer* buffer, bool clearColour, bool clearDepth, bool clearStencil);
			void ClearBuffer(bool colour, bool depth, bool stencil);

			PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
			void DrawMesh(PS4Mesh& mesh);

		protected:
			const int _GarlicMemory; // Amount of Garlic memory
			const int _OnionMemory; // Amount of Onion memory
			const int _MaxCMDBufferCount; // Number of command buffers;
			const int _bufferCount; // Number of screen buffers
			
			int videoHandle; // Handle to video system
			int framesSubmitted; // Total number of frames
			
			int currentGPUBuffer; // index of command buffer
			int currentScreenBuffer; // index of current buffer
			int prevScreenBuffer; // index of previous buffer
			PS4ScreenBuffer** screenBuffers; // Pointer to our screen buffers
			
			sce::Gnmx::Toolkit::StackAllocator stackAllocators[MEMORYMAX];
			
			PS4Shader* defaultShader;
			PS4Mesh* defaultMesh;
			PS4Texture* defaultTexture;
			
			// These are pointers to parts of our current command list
			PS4Frame* frames;
			PS4ScreenBuffer * currentPS4Buffer;
			Gnmx::GnmxGfxContext * currentGFXContext;
			PS4Frame* currentFrame;
		};
	}
}