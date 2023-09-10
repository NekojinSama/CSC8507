#pragma once
#include "ShaderBase.h"
#include "PS4MemoryAware.h"
#include <gnmx.h>
#include <gnmx/shader_parser.h>
#include "..\samples\sample_code\graphics\api_gnm\toolkit\shader_loader.h"
#include "..\samples\sample_code\graphics\api_gnm\toolkit\embedded_shader.h"

using namespace sce;

namespace NCL {
	namespace Rendering {
		class PS4Shader : public NCL::Rendering::ShaderBase, public PS4MemoryAware {
		friend class PS4RendererBase;
		public:
			~PS4Shader();
			static PS4Shader * GenerateShader(const string & vertex, const string & pixel);
			void ReloadShader() override;
		
		protected:
			PS4Shader();
			
			void GenerateVertexShader(const string & name, bool makeFetch);
			void GeneratePixelShader(const string & name);
			void GenerateFetchShader(char* binData);
			
			bool LoadShaderBinary(const string & name, char*& into, int& dataSize);
			bool ShaderIsBinary(const string & name);
			
			void SubmitShaderSwitch(Gnmx::GnmxGfxContext & cmdList);
			int GetConstantBuffer(const string & name);
		protected:
			void* fetchShader;
			
			sce::Shader::Binary::Program vertexBinary;
			sce::Shader::Binary::Program pixelBinary;
			
			Gnmx::VsShader * vertexShader;
			Gnmx::InputOffsetsCache vertexCache;
			
			Gnmx::PsShader * pixelShader;
			Gnmx::InputOffsetsCache pixelCache;
		};
	}
}