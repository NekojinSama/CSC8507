#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

using std::vector;

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject {
	public:
		GameObject(std::string name = "");
		~GameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		RenderObject* GetWireframeObject() const {
			return wireframeVolume;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetWireframeObject(RenderObject* newObject) {
			wireframeVolume = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		void SetName(const std::string& val) {
			name = val;
		}

		int GetScore() {
			return objectScore;
		}

		virtual void OnCollisionBegin(GameObject* otherObject) {}

		virtual void OnCollisionEnd(GameObject* otherObject) {}

		bool GetBroadphaseAABB(Vector3& outsize) const;

		void UpdateBroadphaseAABB();

		Transform& GetParentTransform();

		Transform& GetWorldTransform();

		Matrix4 GetWorldMatrix();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

		void SetLayer(int id) {
			layerID = id;
		}

		int GetLayer() const {
			return layerID;
		}

		void SetPoint(bool val) {
			point = val;
		}

		bool GetPoint() {
			return point;
		}

		void IncreaseScore(int val) {
			objectScore += val;
		}

		int AddScore() {
			int tmp = objectScore;
			objectScore = 0;
			return tmp;
		}

		void AddParent(GameObject* p) {
			parent = p;
			//boundingVolume->UpdateDimensions();
		}

		void AddChild(GameObject* c) {
			children.emplace_back(c);
		}

		bool HasParent() {
			if(parent!=nullptr) return true;
			return false;
		}

	protected:
		Transform			transform;

		CollisionVolume* boundingVolume;
		PhysicsObject* physicsObject;
		RenderObject* renderObject;
		RenderObject* wireframeVolume;
		NetworkObject* networkObject;

		bool		isActive;
		int			worldID;
		int			layerID;
		bool		point;
		int			objectScore = 0;
		std::string	name;

		Vector3 broadphaseAABB;

		GameObject* parent;
		std::vector<GameObject*> children;
	};
}

