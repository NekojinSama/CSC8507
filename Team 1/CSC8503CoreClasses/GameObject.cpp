#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName) {
	name = objectName;
	worldID = -1;
	layerID = 0;
	isActive = true;
	boundingVolume = nullptr;
	physicsObject = nullptr;
	renderObject = nullptr;
	wireframeVolume = nullptr;
	networkObject = nullptr;
	point = false;
	parent = nullptr;
	transform.parent = this;
	objectScore = 0;
}

GameObject::~GameObject() {
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete wireframeVolume;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3& outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

Transform& GameObject::GetParentTransform() {
	return parent->GetTransform();
}

Transform& GameObject::GetWorldTransform() {
	if (parent != nullptr)
		return parent->GetWorldTransform() + transform;
	else
		return transform;
};

Matrix4 GameObject::GetWorldMatrix() {
	if (parent != nullptr)
		return parent->GetWorldMatrix() * transform.GetMatrix();
	else
		return transform.GetMatrix();
};