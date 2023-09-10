#pragma once
#include "MeshGeometry.h"
#include "PS4MemoryAware.h"

#include <gnm.h>

#include <gnmx\context.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <gnm\dataformats.h>

using namespace sce;

namespace NCL {
	namespace Rendering {
		class PS4Mesh : public MeshGeometry, public PS4MemoryAware {
			friend class PS4RendererBase;
		public:
			static PS4Mesh* GenerateQuad();
			void UploadToGPU() override;
			~PS4Mesh();
		protected:
			void SubmitPreDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
			void SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
			//void BufferData();
			void InitAttributeBuffer(sce::Gnm::Buffer& buffer, Gnm::DataFormat format, void* offset);
			PS4Mesh();
		protected:
			struct MeshVertex //Gpu simply has a 4 byte alignment!
			{
				float position[3];
				float textureCoord[2];
				float normal[3];
				float tangent[3];
			};

			Gnm::IndexSize indexType;
			Gnm::PrimitiveType primitiveType;

			int* indexBuffer;
			MeshVertex* vertexBuffer;

			int vertexDataSize;
			int indexDataSize;

			sce::Gnm::Buffer* attributeBuffers;
			int attributeCount;
		};
	}
}