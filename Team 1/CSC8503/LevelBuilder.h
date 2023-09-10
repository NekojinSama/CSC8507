#pragma once
#include "MeshMaterial.h"

namespace NCL {
	namespace Maths {
		class Vector3;
	}


	namespace CSC8503 {
		class GameObject;
		class GameWorld;
		class Transform;

		class LevelBuilder {
		public:
			LevelBuilder() {}
			~LevelBuilder() {}

			void LoadLevel(GameWorld* world);

		private:
			GameObject* CreateObject(GameWorld* world, std::string, Transform);
			Transform CreateTransform(std::vector<std::string>);
			void LoadObjects(GameWorld* world, GameObject* parent, std::ifstream&);

			GameObject* AddSphereToWorld(GameWorld* world, const ::NCL::Maths::Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCapsuleToWorld(GameWorld* world, const ::NCL::Maths::Vector3& position, float radius, float halfHeight, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(GameWorld* world, const ::NCL::Maths::Vector3& position, ::NCL::Maths::Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddOBBToWorld(GameWorld* world, const ::NCL::Maths::Vector3& position, ::NCL::Maths::Vector3 orientation, ::NCL::Maths::Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddComplexMeshToWorld(GameWorld* world, const ::NCL::Maths::Vector3& position, ::NCL::Maths::Vector3 dimensions, ::NCL::Maths::Vector3 orientaiton, float inverseMass, MeshMaterial* mat);
			
		};
	}
}