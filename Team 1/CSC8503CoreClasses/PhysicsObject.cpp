#define _USE_MATH_DEFINES
#include <math.h>
#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "Transform.h"
using namespace NCL;
using namespace CSC8503;

PhysicsObject::PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume) {
	transform = parentTransform;
	volume = parentVolume;

	inverseMass = 1.0f;
	elasticity = 0.8f;
	friction = 0.1f;
	isSpring = false;
	feelsGravity = true;
	wakeTimer = 5;
	awake = true;
	maxVel = Vector3(50, 20, 50);
}

PhysicsObject::~PhysicsObject() {

}

void PhysicsObject::ApplyAngularImpulse(const Vector3& force) {
	angularVelocity += inverseInteriaTensor * force;
}

void PhysicsObject::ApplyLinearImpulse(const Vector3& force) {
	linearVelocity += force * inverseMass;
}

void PhysicsObject::AddForce(const Vector3& addedForce) {
	force += addedForce;
}

void PhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	Vector3 localPos = position - transform->GetPosition();

	force += addedForce;
	torque += Vector3::Cross(localPos, addedForce);
}

void PhysicsObject::AddExtForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	Vector3 localPos = position - transform->GetPosition();

	extForce += addedForce;
	extTorque += Vector3::Cross(localPos, addedForce);
}

void PhysicsObject::AddTorque(const Vector3& addedTorque) {
	torque += addedTorque;
}

void PhysicsObject::SetTorque(const Vector3& setTorque) {
	torque = setTorque;
}

void PhysicsObject::ClearForces() {
	force = Vector3();
	torque = Vector3();
}

void PhysicsObject::ClearExtForces() {
	extForce = Vector3();
	extTorque = Vector3();
}

void PhysicsObject::InitCubeInertia() {
	Vector3 dimensions = transform->GetScale();

	Vector3 fullWidth = dimensions * 2;

	Vector3 dimsSqr = fullWidth * fullWidth;

	inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
	inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
	inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
}

void PhysicsObject::InitSphereInertia(bool hollow) {
	float radius = transform->GetScale().GetMaxElement();
	float i = 2.5f * inverseMass / (radius * radius);

	inverseInertia = Vector3(i, i, i);

	if (hollow) {
		float iz = 0.66f * inverseMass / (radius * radius);

		inverseInertia = Vector3(i, i, iz);
	}
}

void PhysicsObject::InitCapsuleInertia() {
	float r = transform->GetScale().x / 2;
	float h = transform->GetScale().y / 2 - r;

	float mcy = h * pow(r, 2) * M_PI;
	float mhs = (2 * pow(r, 3) * M_PI) / 3;
	float m = mcy + 2 * mhs;

	float ix = mcy * (pow(h, 2) / 12 + pow(r, 2) / 4) + 2 * mhs * (2 * pow(r, 2) / 5 + pow(h / 2, 2) + 3 * h * r / 8);
	float iy = mcy * (pow(r, 2) / 2) + 2 * mhs * (2 * pow(r, 2) / 5);
	float iz = mcy * (pow(h, 2) / 12 + pow(r, 2) / 4) + 2 * mhs * (2 * pow(r, 2) / 5 + pow(h / 2, 2) + 3 * h * 3 / 8);

	inverseInertia = Vector3(1 / ix, 1 / iy, 1 / iz);
}

void PhysicsObject::UpdateInertiaTensor() {
	Quaternion q = transform->GetOrientation();

	Matrix3 invOrientation = Matrix3(q.Conjugate());
	Matrix3 orientation = Matrix3(q);

	inverseInteriaTensor = orientation * Matrix3::Scale(inverseInertia) * invOrientation;
}