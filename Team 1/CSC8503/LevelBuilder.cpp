#include "LevelBuilder.h"
#include "AssetManager.h"

#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "Assets.h"

#include <Quaternion.h>
#include <Vector3.h>

#include <fstream>

using namespace std;
using namespace NCL;
using namespace CSC8503;

std::vector<std::string> Parse(std::string word, char delimiter) {
	vector<string> parsed;
	while (word.contains('('))
		word.at(word.find('(')) = ' ';
	while (word.contains(')'))
		word.at(word.find(')')) = ' ';
	while (word.contains(delimiter)) {
		parsed.emplace_back(word.substr(0, word.find(delimiter)));
		word = word.substr(word.find(delimiter) + 1, word.length() - 1);
	}
	parsed.emplace_back(word);
	return parsed;
}

Transform LevelBuilder::CreateTransform(std::vector<std::string> words) {
	Transform t;
	vector<string> stringVector3 = Parse(words[1], ',');
	t.SetPosition(Vector3(stof(stringVector3[0]), stof(stringVector3[1]), stof(stringVector3[2])));
	stringVector3.clear();

	stringVector3 = Parse(words[2], ',');
	t.SetScale(Vector3(stof(stringVector3[0]) * 0.5f, stof(stringVector3[1]) * 0.5f, stof(stringVector3[2]) * 0.5f));
	stringVector3.clear();

	stringVector3 = Parse(words[3], ',');
	Quaternion q = Quaternion(stof(stringVector3[0]), stof(stringVector3[1]), stof(stringVector3[2]), stof(stringVector3[3]));
	t.SetOrientation(q);
	return t;
}

GameObject* LevelBuilder::CreateObject(GameWorld* world, std::string mesh, Transform transform) {
	GameObject* g = new GameObject();

	MeshGeometry* m = nullptr;

	if (mesh == "Cube")
		m = AssetManager::complexMesh;
	else if (mesh == "Capsule")
		m = AssetManager::capsuleMesh;
	else if (mesh == "Sphere")
		m = AssetManager::sphereMesh;

	// Hacky fix for ramps because the new cube doesn't work
	if (transform.GetOrientation().x != 0 || transform.GetOrientation().z != 0) {
		g->GetTransform().SetOrientation(transform.GetOrientation());
		m = AssetManager::cubeMesh;
	}

	g->GetTransform().SetPosition(transform.GetPosition());
	g->GetTransform().SetScale(transform.GetScale());

	Quaternion identityY = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	Quaternion identityX = Quaternion(0.0f, 1.0f, 0.0f, 0.0f);

	if (m != nullptr) {
		if (transform.GetOrientation() == identityX || transform.GetOrientation() == identityY) {
			AABBVolume* volume = new AABBVolume(transform.GetScale() / 2);
			g->SetBoundingVolume((CollisionVolume*)volume);
			g->SetWireframeObject(new RenderObject(&g->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::wireframeShader));
		}
		else {
			OBBVolume* volume = new OBBVolume(transform.GetScale() / 2);
			g->SetBoundingVolume((CollisionVolume*)volume);
			g->SetWireframeObject(new RenderObject(&g->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::wireframeShader));
		}

		if (m == AssetManager::complexMesh) {
			g->SetRenderObject(new RenderObject(&g->GetTransform(), m, AssetManager::basicTex, AssetManager::basicShader, true, AssetManager::complexMat));
			if (g->GetRenderObject()->isComplex) {
				g->GetRenderObject()->GetTransform()->SetScale(g->GetRenderObject()->GetTransform()->GetScale() / 2);
			}
		}
		else {
			g->SetRenderObject(new RenderObject(&g->GetTransform(), m, AssetManager::basicTex, AssetManager::basicShader));
		}

		g->SetPhysicsObject(new PhysicsObject(&g->GetTransform(), g->GetBoundingVolume()));

		g->GetPhysicsObject()->SetInverseMass(0);
		g->GetPhysicsObject()->InitCubeInertia();
	}

	world->AddGameObject(g);

	return g;
}

void LevelBuilder::LoadObjects(GameWorld* world, GameObject* parent, ifstream& file) {
	string line;
	getline(file, line);

	while (line != "#") {
		if (line == "/") {
			LoadObjects(world, parent, file);
			return;
		}
		else {
			vector<string> parsed = Parse(line, '_');
			GameObject* g = CreateObject(world, parsed[0], CreateTransform(parsed));
			g->AddParent(parent);
			parent->AddChild(g);
			LoadObjects(world, g, file);
		}
		getline(file, line);
	}
}

void LevelBuilder::LoadLevel(GameWorld* world) {
	std::ifstream inFile(Assets::DATADIR + "Out.txt");
	string firstLine;
	getline(inFile, firstLine);

	vector<string> parsed = Parse(firstLine, '_');
	GameObject* root = CreateObject(world, parsed[0], CreateTransform(parsed));
	LoadObjects(world, root, inFile);

	inFile.close();
}

GameObject* LevelBuilder::AddSphereToWorld(GameWorld* world, const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->SetWireframeObject(new RenderObject(&sphere->GetTransform(), AssetManager::sphereMesh, AssetManager::basicTex, AssetManager::wireframeShader));

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), AssetManager::sphereMesh, AssetManager::basicTex, AssetManager::basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* LevelBuilder::AddCapsuleToWorld(GameWorld* world, const Vector3& position, float radius, float halfHeight, float inverseMass) {
	GameObject* capsule = new GameObject();

	Vector3 capsuleSize = Vector3(radius * 2, halfHeight, radius * 2);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->SetWireframeObject(new RenderObject(&capsule->GetTransform(), AssetManager::capsuleMesh, AssetManager::basicTex, AssetManager::wireframeShader));

	capsule->GetTransform()
		.SetScale(capsuleSize)
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), AssetManager::capsuleMesh, AssetManager::basicTex, AssetManager::basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* LevelBuilder::AddCubeToWorld(GameWorld* world, const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions / 2);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->SetWireframeObject(new RenderObject(&cube->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* LevelBuilder::AddOBBToWorld(GameWorld* world, const Vector3& position, Vector3 orientation, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	OBBVolume* volume = new OBBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->SetWireframeObject(new RenderObject(&cube->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::wireframeShader));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(orientation.x, orientation.y, orientation.z));

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* LevelBuilder::AddComplexMeshToWorld(GameWorld* world, const Vector3& position, Vector3 dimensions, Vector3 orientation, float inverseMass, MeshMaterial* mat) {
	GameObject* cube = new GameObject();

	OBBVolume* volume = new OBBVolume(dimensions / 2);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	//cube->SetWireframeObject(new RenderObject(&cube->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::wireframeShader));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions / 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(orientation.x, orientation.y, orientation.z));

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), AssetManager::complexMesh, AssetManager::basicTex, AssetManager::basicShader, true, AssetManager::complexMat));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}