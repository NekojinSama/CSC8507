#include "Transform.h"
#include "GameObject.h"

using namespace NCL::CSC8503;

Transform::Transform()	{
	scale = Vector3(1, 1, 1);
}

Transform::~Transform()	{

}

void Transform::UpdateMatrix() {
	matrix =
		Matrix4::Translation(position) *
		Matrix4(orientation) *
		Matrix4::Scale(scale);
}

Transform& Transform::SetPosition(const Vector3& worldPos) {
	position = worldPos;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetScale(const Vector3& worldScale) {
	scale = worldScale;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetOrientation(const Quaternion& worldOrientation) {
	orientation = worldOrientation;
	UpdateMatrix();
	return *this;
}

Transform Transform::GetWorldTransform() {
	if(parent->HasParent()){
		//parent->GetParentTransform().GetWorldTransform();
		//Transform& t = *this + parent->GetParentTransform().GetWorldTransform();
		//t.UpdateMatrix();
		return *this + parent->GetParentTransform().GetWorldTransform();
	}
	else
		return *this;

}

Transform& Transform::operator +(const Transform& t)	const {
	Transform transform;
	transform.position = t.position + (t.GetOrientation() * this->position);
	transform.orientation = t.GetOrientation() * this->GetOrientation();
	//scale needs to be mulyiplied by parents, but causes issues with bounding volumes
	transform.scale = this->GetScale();
	transform.UpdateMatrix();
	return transform;
}