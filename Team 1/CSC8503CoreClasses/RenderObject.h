#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"
#include "MeshMaterial.h"

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, bool hasMask = true, MeshMaterial* mat = nullptr);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			TextureBase* GetPaintMaskTexture() const {
				return paintMask;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void SetPaintCentre(Vector3 uvCoords) {
				paintCentre = uvCoords;
			}

			Vector3 GetPaintCentre() const {
				return paintCentre;
			}

			//For imported assets with materials:
			bool isComplex;

			void AddMeshMaterial(MeshMaterial* mat) {
				material = mat;
			}

			MeshMaterial* GetMeshMaterial() const {
				return material;
			}

		protected:
			MeshGeometry*	mesh;
			MeshMaterial*	material;
			TextureBase*	texture;
			TextureBase*	paintMask;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
			Vector3			paintCentre;
		};
	}
}
