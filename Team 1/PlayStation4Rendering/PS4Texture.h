#pragma once
#include "TextureBase.h"
#include <string>
#include <gnm\texture.h>
#include "PS4MemoryAware.h"
#include <gnf.h>

namespace NCL {
	namespace Rendering {
		class PS4Texture : public NCL::Rendering::TextureBase, public PS4MemoryAware {
		public:
			friend class PS4RendererBase;
			~PS4Texture() {}
			static PS4Texture* LoadTextureFromFile(const std::string& filename);

			const sce::Gnm::Texture& GetAPITexture() const { return apiTexture; }
		protected:
			PS4Texture() {}
			sce::Gnm::Texture apiTexture;
		};
	}
}