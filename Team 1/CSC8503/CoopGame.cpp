#include "CoopGame.h"
#include "RenderObject.h"
#include "HUD.h"


CoopGame::CoopGame(GameWorld* w, GameTechRenderer* r) : TutorialGame(w, r) {
	world->RemoveExtraCameras();
}

CoopGame::~CoopGame() {
	players.clear();
}

/*
need to add new innit cameras for multiple cameras
add mutliple players
redo inti world for this
loop through update for each player

then call start coop
*/

Player* CoopGame::AddPlayerToWorld(const Vector3& position, const int id, const int controller, const int team){
	float meshSize = 1.0f;
	float inverseMass = 0.9f;

	Player* p = new Player(world, this, team, id, controller);

	SphereVolume* volume = new SphereVolume(meshSize);

	p->SetBoundingVolume((CollisionVolume*)volume);

	p->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	p->SetRenderObject(new RenderObject(&p->GetTransform(), AssetManager::charMesh, AssetManager::basicTex, AssetManager::basicShader, false));
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));

	p->GetPhysicsObject()->SetInverseMass(inverseMass);
	p->GetPhysicsObject()->InitSphereInertia();

	if (team == 0) {
		p->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	}
	else {
		p->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	}

	world->AddGameObject(p);

	return p;
}

void CoopGame::InitCamera(int numPlayers) {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	
	world->AddNewCamera();
	world->GetCamera(1)->SetNearPlane(0.1f);
	world->GetCamera(1)->SetFarPlane(500.0f);
	world->GetCamera(1)->SetPitch(-15.0f);
	world->GetCamera(1)->SetYaw(315.0f);
}

void CoopGame::StartCoop(int numPlayers) {
	state = 1;
	InitWorld();
	InitCamera(numPlayers);

	//need to add two local players
	players.emplace_back(AddPlayerToWorld({20, 2, 0}, 0, 0, 0));
	//players.front()->SetRenderObject(nullptr);
	
	players.emplace_back(AddPlayerToWorld({-20, 2,0 }, 1, 1, 1));
	//players.back()->SetRenderObject(nullptr);

	canPause = true;
	currentObjective = AddObjectiveToWorld(Vector3(10, 3, 10), Vector3(), { 1, 25, 1 });
	renderer->SetSSAOToggle(false);

	vector<Player*> allPlayers;
	for (std::list<Player*>::iterator it = players.begin(); it != players.end(); ++it) {
		allPlayers.push_back(*it);
	}
	AddAllAI(allPlayers);
}

void CoopGame::UpdateGame(float dt) {
	if (state == 1) {
		for (auto& i : players) {
			i->Update(dt);
		}
		hud->setPlayerList(players);
		hud->CoopGameWeaponHUD(renderer);
	}
	
	TutorialGame::UpdateGame(dt);
}