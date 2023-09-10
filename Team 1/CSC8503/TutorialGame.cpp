#include "TutorialGame.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "MainMenu.h"
#include "LevelBuilder.h"
#include "AIEntity.h"
#include "HUD.h"
#include "psapi.h"

using namespace std;
using namespace NCL;
using namespace CSC8503;

static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

TutorialGame::TutorialGame(GameWorld* w, GameTechRenderer* r) : memoryInfo() {
	world		= w;
#ifdef USEVULKAN
	renderer		= new GameTechVulkanRenderer(*world);
#else 
	renderer = r;
#endif

	physics			= new PhysicsSystem(*world);

	canPause = false;

	timer = 300.0f;

	state = 0;

	scores[0] = 0;
	scores[1] = 0;
	currentObjective = nullptr;
	localPlayer = nullptr;

	levelLoader = new LevelBuilder();;

	hud = new HUD(this);
	renderer->SetHUD(hud);

	averageFrameRate = 0.0f;
	for (int i = 0; i < 10; i++) { frameTimes[i] = 0; }
	newestTime = 0;

	sphereCheckObj = new GameObject();
	Vector3 sphereSize = Vector3(3, 3, 3);
	SphereVolume* volume = new SphereVolume(3);
	sphereCheckObj->SetBoundingVolume((CollisionVolume*)volume);
	sphereCheckObj->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(Vector3());

	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));


	// refactor me
	lavaTex = renderer->LoadTexture("noise_4.png");
	lavaTex2 = renderer->LoadTexture("noise_5.png");
	flagShader = renderer->LoadShader("flag.vert", "flag.frag");
	edgeShader = renderer->LoadShader("scene.vert", "edge.frag");
	lavaShader = renderer->LoadShader("effect1.vert", "effect1.frag");
	gateShader = renderer->LoadShader("gate.vert", "gate.frag");
	
	

}

TutorialGame::~TutorialGame()	{	
	world->ClearAndErase();

	delete physics;
	delete sphereCheckObj;
	delete levelLoader;
	delete hud;
	delete navMesh;

	physics = nullptr;
	localPlayer = nullptr;
	currentObjective = nullptr;
	sphereCheckObj = nullptr;
	levelLoader = nullptr;
	hud = nullptr;
	navMesh = nullptr;
}

void TutorialGame::UpdateGame(float dt) {
	frameTimes[newestTime] = dt;
	averageFrameRate = frameTimes[newestTime];
	newestTime = (newestTime + 1) % 10;
	
	
	PROCESS_MEMORY_COUNTERS_EX pmc;
	memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryInfo);
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P) && canPause) {
		state = 2;
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::M) && canPause) {
		state = state == 3 ? 1 : 3;
		
	}

	switch (state) {
	case 0:
		renderer->Render(3);
		break;
	case 1:
		timer -= dt;
		if (localPlayer != nullptr) {
			localPlayer->Update(dt);
			hud->WeaponHUD(renderer, localPlayer);
		}
		world->UpdateWorld(dt);
		//renderer->Update(dt);
		physics->Update(dt);
		UpdateAI(dt);
		//hud->GameHUD(renderer);
		renderer->Render(0);
		if (timer <= 0 && canPause) {
			//state = scores[localPlayer->GetTeam()] > scores[(localPlayer->GetTeam() - 1) % 2] ? 4 : 5;
			state = 4;
		}
		break;
	case 2:
		renderer->Render(2);
		break;
	case 3:
		world->GetMainCamera()->UpdateCamera(dt);

		for (int i = 0; i < 10; i++) {
			averageFrameRate += frameTimes[i];
		}
		averageFrameRate /= 10.0f;
		RAMUsage = pmc.WorkingSetSize;
		VirtualMemUsage = pmc.PrivateUsage;
		CPUUsage = GetCurrentCPU();
		physicsTimer = physics->GetPhysicsTimer();
		hud->DebugHUD(renderer);
		renderer->Render(6);
		break;
	case 4:
		winner = scores[0] == scores[1] ? 0 : (scores[0] > scores[1] ? 1 : 2);
		renderer->Render(4);
		break;
	case 5:
		renderer->Render(5);
		break;
	}

	SoundSystem::GetSoundSystem()->Update(dt);

	Debug::UpdateRenderables(dt);
	
	//RaisingFlag(dt);
	//RaisingAnoFlag(dt);
	
	

	//ResetStatePosition();
	//JudgeTeamSecore();

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void TutorialGame::InitWorld() {
	
	world->ClearAndErase();
	physics->Clear(); 

	
	PutStateMachine();
	//MovingPole();
	MovingPool(Vector3(205, 15, -275));
	MovingPool2(Vector3(205, 16, -275));
	MovingPoolSecond(Vector3(-205, 15, 275));
	MovingPoolSecond2(Vector3(-205, 15, 275));
	
	
	levelLoader->LoadLevel(world);
	navMesh = new NavigationMesh("level1navMesh.navmesh");
}

void TutorialGame::UpdateAI(float dt) {
	UpdateStateMachine(dt);
	BallsGoBack(ballsta);
	BallsGoBack2(ballsta2);

	for (int i = 0; i < aiEntities.size(); i++) {
		aiEntities[i]->Update(dt);
	}
}

void TutorialGame::StartSolo() {
	state = 1;
	InitWorld();
	InitCamera();
	localPlayer = AddPlayerToWorld({ 0, 5, 0 }, 0, 0);
	localPlayer->SetRenderObject(nullptr);
	canPause = true;
	currentObjective = AddObjectiveToWorld(Vector3(10, 3, 10), Vector3(), {1, 25, 1});
	vector<Player*> allPlayers;
	allPlayers.push_back(localPlayer);
	AddAllAI(allPlayers);
}

GameObject* TutorialGame::AddBasicLevel(const Vector3& position) {
	GameObject* level = new GameObject();

	Vector3 levelSize = Vector3(1, 1, 1);
	AABBVolume* volume = new AABBVolume(levelSize);
	level->SetBoundingVolume((CollisionVolume*)volume);
	level->GetTransform()
		.SetScale(levelSize * 2)
		.SetPosition(position);

	level->SetRenderObject(new RenderObject(&level->GetTransform(), AssetManager::basicLevelMesh, AssetManager::basicTex, AssetManager::basicShader));
	level->SetPhysicsObject(new PhysicsObject(&level->GetTransform(), level->GetBoundingVolume()));

	level->GetPhysicsObject()->SetInverseMass(0);
	level->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(level);

	return level;
}

Player* TutorialGame::AddPlayerToWorld(const Vector3& position, const int id, const int team) {
	float meshSize = 1.0f;
	float inverseMass = 0.9f;

	Player* p = new Player(world, this, id, team, world->GetMainCamera());

	SphereVolume* volume = new SphereVolume(1.0f);

	p->SetBoundingVolume((CollisionVolume*)volume);

	p->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	p->SetRenderObject(new RenderObject(&p->GetTransform(), AssetManager::charMesh, AssetManager::basicTex, AssetManager::basicShader, false));
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), p->GetBoundingVolume()));

	p->GetPhysicsObject()->SetInverseMass(inverseMass);
	p->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(p);

	if (team == 0) {
		p->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
	}
	else {
		p->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	}

	return p;
}

AIEntity* TutorialGame::AddAIToWorld(const Vector3& position, Vector2 bounds, vector<Player*> allPlayers) {
	AIEntity* ai = new  AIEntity(position, navMesh, bounds, allPlayers, world, this);

	float radius = 1.5f;
	float halfHeight = 3.0f;

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	ai->SetBoundingVolume((CollisionVolume*)volume);

	ai->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	ai->SetRenderObject(new RenderObject(&ai->GetTransform(), AssetManager::capsuleMesh, AssetManager::basicTex, AssetManager::basicShader));
	ai->SetPhysicsObject(new PhysicsObject(&ai->GetTransform(), ai->GetBoundingVolume()));

	ai->GetPhysicsObject()->SetInverseMass(2.0f);
	ai->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(ai);

	ai->Start();

	return ai;
}

Objective* TutorialGame::AddObjectiveToWorld(const Vector3& position, Vector3 orientation, Vector3 dimensions, float inverseMass) {
	Objective* obj = new Objective(&scores[0], &scores[1]);

	OBBVolume* volume = new OBBVolume(dimensions);
	obj->SetBoundingVolume((CollisionVolume*)volume);

	obj->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(orientation.x, orientation.y, orientation.z));

	obj->SetRenderObject(new RenderObject(&obj->GetTransform(), AssetManager::cylinderMesh, AssetManager::basicTex, AssetManager::basicShader));

	obj->GetRenderObject()->SetColour(Vector4(0, 0.5, 0.5, 1));

	obj->flag = BindFlag(obj);

	movingfl = obj;
	world->AddGameObject(obj);

	return obj;

	
}

void TutorialGame::AddAllAI(vector<Player*> allPlayers) {
	aiEntities.push_back(AddAIToWorld(Vector3(-10, 3, 100), Vector2(40, 40), allPlayers));

	aiEntities.push_back(AddAIToWorld(Vector3(10, 3, -100), Vector2(30, 30), allPlayers));
}

void TutorialGame::ApplyPaint(Vector3 colour, Vector3 position, Vector3 direction, GameObject* ignore) {
	Ray ray = Ray(position, direction);
	RayCollision closestCollision;

	if (world->Raycast(ray, closestCollision, true, ignore) && closestCollision.rayDistance < 150.0f) {

		Vector3 centre = closestCollision.collidedAt;
		sphereCheckObj->GetTransform().SetPosition(centre);

		std::vector<GameObject*> hitObjects;

		std::vector<GameObject*>::const_iterator first;
		std::vector<GameObject*>::const_iterator last;
		world->GetObjectIterators(first, last);
		for (auto i = first; i != last; i++) {
			CollisionDetection::CollisionInfo info;
			if (CollisionDetection::ObjectIntersection(sphereCheckObj, *i, info) && *i != localPlayer) {
				if ((*i)->GetRenderObject()->GetPaintMaskTexture() == nullptr) continue;
				hitObjects.push_back(*i);
			}
		}

		AssetManager::paintShader->Bind();
		
		for (auto& i : hitObjects) {
			TextureBase* paintMask = i->GetRenderObject()->GetPaintMaskTexture();

			OGLMesh* mesh = (OGLMesh*)(i)->GetRenderObject()->GetMesh();
			GLuint vaoID = mesh->GetVAO();
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->attributeBuffers[VertexAttribute::Positions]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mesh->attributeBuffers[VertexAttribute::TextureCoords]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mesh->indexBuffer);


			glBindImageTexture(0, ((OGLTexture*)paintMask)->GetObjectID(), 0, GL_FALSE, NULL, GL_WRITE_ONLY, GL_RGBA32F);
			int uvLoc = glGetUniformLocation(AssetManager::paintShader->GetProgramID(), "paintCentre");
			Vector3 localSpacePos = i->GetTransform().GetMatrix().Inverse() * closestCollision.collidedAt;
			glUniform3fv(uvLoc, 1, (float*)&localSpacePos);

			int radiusLoc = glGetUniformLocation(AssetManager::paintShader->GetProgramID(), "radius");
			int radius = 4;
			glUniform1i(radiusLoc, radius);

			int rayDirLoc = glGetUniformLocation(AssetManager::paintShader->GetProgramID(), "rayDir");
			Vector3 dir = -ray.GetDirection();
			glUniform3fv(rayDirLoc, 1, (float*)&dir);

			int colourLoc = glGetUniformLocation(AssetManager::paintShader->GetProgramID(), "paintColour");
			glUniform3fv(colourLoc, 1, (float*)&colour);

			AssetManager::paintShader->Execute((mesh->GetIndexCount() / 3) / 64 + 1, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

			AssetManager::paintShader->Unbind();
	}

}

std::vector<StateGameObject*> TutorialGame::TestStateMachine(const std::vector<Vector3>& positions)
{
	std::vector<StateGameObject*> statemachines;
	for (int i = 0; i < positions.size(); ++i) {
		StateGameObject* apple = new StateGameObject();

		AABBVolume* volume = new AABBVolume(Vector3(5,1,5));
		apple->SetBoundingVolume((CollisionVolume*)volume);
		apple->GetTransform()
			.SetScale(Vector3(30,30,30))
			.SetPosition(positions.at(i));

		apple->SetRenderObject(new RenderObject(&apple->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

		apple->GetPhysicsObject()->SetInverseMass(0.1f);
		apple->GetPhysicsObject()->InitSphereInertia();
		apple->GetPhysicsObject()->FeelsGravity(0);
		//apple->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	
		statemachines.push_back(apple);
		world->AddGameObject(apple);

	}
	

	return statemachines;
}

std::vector<StateGameObject2*> TutorialGame::TestStateMachineSecond(const std::vector<Vector3>& positions)
{
	std::vector<StateGameObject2*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		StateGameObject2* apple = new StateGameObject2();
		AABBVolume* volume = new AABBVolume(Vector3(5, 1, 5));
		apple->SetBoundingVolume((CollisionVolume*)volume);
		apple->GetTransform()
			.SetScale(Vector3(30,30,30))
			.SetPosition(positions.at(i));

		apple->SetRenderObject(new RenderObject(&apple->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

		apple->GetPhysicsObject()->SetInverseMass(0.1f);
		apple->GetPhysicsObject()->InitSphereInertia();
		apple->GetPhysicsObject()->FeelsGravity(0);
		//apple->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));

		statemachines.push_back(apple);
		world->AddGameObject(apple);
	}
	return statemachines;
}

std::vector<TestStateThree*> TutorialGame::ThreeStatesMachine(const std::vector<Vector3>& positions)
{
	std::vector<TestStateThree*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TestStateThree* three = new TestStateThree();

		OBBVolume* volume = new OBBVolume(Vector3(2.5, 10, 5.5));
		three->SetBoundingVolume((CollisionVolume*)volume);
		three->GetTransform().SetScale(Vector3(5,20,15)).SetPosition(positions.at(i));

		three->SetRenderObject(new RenderObject(&three->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		three->SetPhysicsObject(new PhysicsObject(&three->GetTransform(), three->GetBoundingVolume()));

		three->GetPhysicsObject()->SetInverseMass(0.001f);
		three->GetPhysicsObject()->InitCubeInertia();
		three->GetPhysicsObject()->FeelsGravity(0);
		//three->GetRenderObject()->SetColour(Vector4(1, 0.5, 0, 1));
		statemachines.push_back(three);

		world->AddGameObject(three);
	}
	return statemachines;
}

std::vector<TestStateThree2*> TutorialGame::ThreeStatesMachineSecond(const std::vector<Vector3>& positions)
{
	std::vector<TestStateThree2*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TestStateThree2* three = new TestStateThree2();
		OBBVolume* volume = new OBBVolume(Vector3(2.5, 10, 5.5));

		three->SetBoundingVolume((CollisionVolume*)volume);
		three->GetTransform().SetScale(Vector3(5,20,15)).SetPosition(positions.at(i));

		three->SetRenderObject(new RenderObject(&three->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		three->SetPhysicsObject(new PhysicsObject(&three->GetTransform(), three->GetBoundingVolume()));

		three->GetPhysicsObject()->SetInverseMass(0.001f);
		three->GetPhysicsObject()->InitCubeInertia();
		three->GetPhysicsObject()->FeelsGravity(0);
		//three->GetRenderObject()->SetColour(Vector4(1, 0.5, 0, 1));
		statemachines.push_back(three);


		world->AddGameObject(three);
	}
	return statemachines;
}

std::vector<TestStateFour*> TutorialGame::FourStatesMachine(const std::vector<Vector3>& positions)
{
	std::vector<TestStateFour*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TestStateFour* four = new TestStateFour();
		AABBVolume* volume = new AABBVolume(Vector3(5, 5, 5));

		four->SetBoundingVolume((CollisionVolume*)volume);
		four->GetTransform().SetScale(Vector3(10, 10, 10)).SetPosition(positions.at(i));

		four->SetRenderObject(new RenderObject(&four->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, gateShader));
		four->SetPhysicsObject(new PhysicsObject(&four->GetTransform(), four->GetBoundingVolume()));

		four->GetPhysicsObject()->SetInverseMass(0.1f);
		four->GetPhysicsObject()->InitCubeInertia();
		four->GetPhysicsObject()->FeelsGravity(0);
		//four->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
		statemachines.push_back(four);

		world->AddGameObject(four);
	}
	return statemachines;
}

std::vector<TestStateFour2*> TutorialGame::FourStatesMachineSecond(const std::vector<Vector3>& positions)
{
	std::vector<TestStateFour2*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TestStateFour2* four = new TestStateFour2();
		AABBVolume* volume = new AABBVolume(Vector3(5, 5, 5));

		four->SetBoundingVolume((CollisionVolume*)volume);
		four->GetTransform().SetScale(Vector3(10, 10, 10)).SetPosition(positions.at(i));

		four->SetRenderObject(new RenderObject(&four->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, gateShader));
		four->SetPhysicsObject(new PhysicsObject(&four->GetTransform(), four->GetBoundingVolume()));

		four->GetPhysicsObject()->SetInverseMass(0.1f);
		four->GetPhysicsObject()->InitCubeInertia();
		four->GetPhysicsObject()->FeelsGravity(0);
		//four->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
		statemachines.push_back(four);

		world->AddGameObject(four);
	}
	return statemachines;
}

std::vector<SpecialState*> TutorialGame::SpecialStatesMachine(const std::vector<Vector3>& positions)
{
	std::vector<SpecialState*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		SpecialState* special = new SpecialState();
		SphereVolume* volume = new SphereVolume(5);

		special->SetBoundingVolume((CollisionVolume*)volume);
		special->GetTransform().SetScale(Vector3(5,5,5)).SetPosition(positions.at(i));

		special->SetRenderObject(new RenderObject(&special->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		special->SetPhysicsObject(new PhysicsObject(&special->GetTransform(), special->GetBoundingVolume()));

		special->GetPhysicsObject()->SetInverseMass(0.1f);
		special->GetPhysicsObject()->InitCubeInertia();
		special->GetPhysicsObject()->FeelsGravity(0);
		//special->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
		statemachines.push_back(special);

		world->AddGameObject(special);
		
	}
	return statemachines;
}

std::vector<DoorState*> TutorialGame::DoorStatesMachine(const std::vector<Vector3>& positions)
{
	std::vector<DoorState*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		DoorState* door = new DoorState();
		AABBVolume* volume = new AABBVolume(Vector3(60, 40, 3));

		door->SetBoundingVolume((CollisionVolume*)volume);
		door->GetTransform().SetScale(Vector3(120, 60, 3)).SetPosition(positions.at(i));

		door->SetRenderObject(new RenderObject(&door->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));

		door->GetPhysicsObject()->SetInverseMass(0.1f);
		door->GetPhysicsObject()->InitCubeInertia();
		door->GetPhysicsObject()->FeelsGravity(0);
		door->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
		statemachines.push_back(door);

		world->AddGameObject(door);
	}
	return statemachines;
}

std::vector<DoorState2*> TutorialGame::DoorStatesMachineSecond(const std::vector<Vector3>& positions)
{
	std::vector<DoorState2*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		DoorState2* door = new DoorState2();
		AABBVolume* volume = new AABBVolume(Vector3(60, 40, 3));

		door->SetBoundingVolume((CollisionVolume*)volume);
		door->GetTransform().SetScale(Vector3(120, 60, 3)).SetPosition(positions.at(i));

		door->SetRenderObject(new RenderObject(&door->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		door->SetPhysicsObject(new PhysicsObject(&door->GetTransform(), door->GetBoundingVolume()));

		door->GetPhysicsObject()->SetInverseMass(0.1f);
		door->GetPhysicsObject()->InitCubeInertia();
		door->GetPhysicsObject()->FeelsGravity(0);
		door->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
		statemachines.push_back(door);

		world->AddGameObject(door);
	}
	return statemachines;
}

std::vector<TheBeat*> TutorialGame::BeatState(const std::vector<Vector3>& positions)
{
	std::vector<TheBeat*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TheBeat* beat = new TheBeat();
		AABBVolume* volume = new AABBVolume(Vector3(10, 0.5, 7.5));

		beat->SetBoundingVolume((CollisionVolume*)volume);
		beat->GetTransform().SetScale(Vector3(20,1,15)).SetPosition(positions.at(i));

		beat->SetRenderObject(new RenderObject(&beat->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		beat->SetPhysicsObject(new PhysicsObject(&beat->GetTransform(), beat->GetBoundingVolume()));

		beat->GetPhysicsObject()->SetInverseMass(0.001f);
		beat->GetPhysicsObject()->InitCubeInertia();
		beat->GetPhysicsObject()->FeelsGravity(0);
		//beat->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
		statemachines.push_back(beat);

		world->AddGameObject(beat);
	}
	return statemachines;
}

std::vector<GameObject*> TutorialGame::BallsState(const std::vector<Vector3>& positions)
{
	std::vector<GameObject*> balls;
	for (int i = 0; i < positions.size(); ++i)
	{
		GameObject* ball = new GameObject();
		SphereVolume* volume = new SphereVolume(10);
		ball->SetBoundingVolume((CollisionVolume*)volume);
		ball->GetTransform().SetScale(Vector3(10,10,10)).SetPosition(positions.at(i));

		ball->SetRenderObject(new RenderObject(&ball->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		ball->SetPhysicsObject(new PhysicsObject(&ball->GetTransform(), ball->GetBoundingVolume()));

		ball->GetPhysicsObject()->SetInverseMass(0.0001f);
		//ball->GetPhysicsObject()->InitSphereInertia(true);
		//ball->GetPhysicsObject()->IsSpring();
		ball->GetPhysicsObject()->SetAngularVelocity(Vector3(-5,0,0));
		
		
		balls.push_back(ball);

		world->AddGameObject(ball);
		
	}
	ballsta = balls;
	return balls;
}

std::vector<GameObject*> TutorialGame::BallsState1(const std::vector<Vector3>& positions)
{
	std::vector<GameObject*> balls;
	for (int i = 0; i < positions.size(); ++i)
	{
		GameObject* ball = new GameObject();
		SphereVolume* volume = new SphereVolume(10);
		ball->SetBoundingVolume((CollisionVolume*)volume);
		ball->GetTransform().SetScale(Vector3(10,10,10)).SetPosition(positions.at(i));

		ball->SetRenderObject(new RenderObject(&ball->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		ball->SetPhysicsObject(new PhysicsObject(&ball->GetTransform(), ball->GetBoundingVolume()));

		ball->GetPhysicsObject()->SetInverseMass(0.0001f);
		ball->GetPhysicsObject()->InitSphereInertia(true);
		//ball->GetPhysicsObject()->IsSpring();
		ball->GetPhysicsObject()->SetAngularVelocity(Vector3(5, 0, 0));


		balls.push_back(ball);

		world->AddGameObject(ball);

	}
	ballsta2 = balls;
	return balls;
}

GameObject* TutorialGame::BindFlag(GameObject* pole)
{
	Vector3 polePos = pole->GetTransform().GetPosition();
	GameObject* flag = new GameObject();
	
	flag->GetTransform().SetScale(Vector3(20, 10, 0.5)).SetPosition(Vector3(polePos.x - 10, polePos.y + 45, polePos.z));
	flag->SetRenderObject(new RenderObject(&flag->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, flagShader));
	flag->SetPhysicsObject(new PhysicsObject(&flag->GetTransform(), flag->GetBoundingVolume()));

	flag->GetPhysicsObject()->SetInverseMass(0.0f);
	flag->GetPhysicsObject()->InitCubeInertia();
	flag->GetRenderObject()->SetColour(Vector3(0, 0.5, 0.5));
	flagT = flag;

	
	world->AddGameObject(flag);

	return flag;
}

GameObject* TutorialGame::MovingPool(const Vector3& position)
{
	GameObject* pool = new GameObject();
	pool->GetTransform().SetScale(Vector3(220, 2, 120)).SetPosition(position);
	pool->SetRenderObject(new RenderObject(&pool->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, lavaShader));
	pool->SetPhysicsObject(new PhysicsObject(&pool->GetTransform(), pool->GetBoundingVolume()));

	pool->GetPhysicsObject()->SetInverseMass(0.0f);
	pool->GetPhysicsObject()->InitCubeInertia();
	pool->GetPhysicsObject()->FeelsGravity(0);
	pool->GetRenderObject()->SetColour(Vector4(0, 0.5, 0.5, 1));


	world->AddGameObject(pool);

	return pool;
}

GameObject* TutorialGame::MovingPool2(const Vector3& position)
{
	GameObject* pool = new GameObject();
	pool->GetTransform().SetScale(Vector3(220, 2, 120)).SetPosition(position);
	pool->SetRenderObject(new RenderObject(&pool->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, lavaShader));
	pool->SetPhysicsObject(new PhysicsObject(&pool->GetTransform(), pool->GetBoundingVolume()));

	pool->GetPhysicsObject()->SetInverseMass(0.0f);
	pool->GetPhysicsObject()->InitCubeInertia();
	pool->GetPhysicsObject()->FeelsGravity(0);
	pool->GetRenderObject()->SetColour(Vector4(0, 0.5, 0.5, 1));


	world->AddGameObject(pool);

	return pool;
}

GameObject* TutorialGame::MovingPoolSecond(const Vector3& position)
{
	GameObject* pool = new GameObject();
	pool->GetTransform().SetScale(Vector3(220, 2, 120)).SetPosition(position);
	pool->SetRenderObject(new RenderObject(&pool->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, lavaShader));
	pool->SetPhysicsObject(new PhysicsObject(&pool->GetTransform(), pool->GetBoundingVolume()));

	pool->GetPhysicsObject()->SetInverseMass(0.0f);
	pool->GetPhysicsObject()->InitCubeInertia();
	pool->GetPhysicsObject()->FeelsGravity(0);
	pool->GetRenderObject()->SetColour(Vector4(0, 0.5, 0.5, 1));


	world->AddGameObject(pool);

	return pool;
}

GameObject* TutorialGame::MovingPoolSecond2(const Vector3& position)
{
	GameObject* pool = new GameObject();
	pool->GetTransform().SetScale(Vector3(220, 2, 120)).SetPosition(position);
	pool->SetRenderObject(new RenderObject(&pool->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, lavaShader));
	pool->SetPhysicsObject(new PhysicsObject(&pool->GetTransform(), pool->GetBoundingVolume()));

	pool->GetPhysicsObject()->SetInverseMass(0.0f);
	pool->GetPhysicsObject()->InitCubeInertia();
	pool->GetPhysicsObject()->FeelsGravity(0);
	pool->GetRenderObject()->SetColour(Vector4(0, 0.5, 0.5, 1));


	world->AddGameObject(pool);

	return pool;
}

std::vector<TheLift*> TutorialGame::LiftStateMachine(const std::vector<Vector3>& positions)
{
	std::vector<TheLift*> statemachines;
	for (int i = 0; i < positions.size(); ++i)
	{
		TheLift* lift = new TheLift();
		AABBVolume* volume = new AABBVolume(Vector3(10, 0.3, 7.5));
		lift->SetBoundingVolume((CollisionVolume*)volume);
		lift->GetTransform().SetScale(Vector3(20, 0.7, 15)).SetPosition(positions.at(i));

		lift->SetRenderObject(new RenderObject(&lift->GetTransform(), AssetManager::cubeMesh, AssetManager::basicTex, AssetManager::basicShader));
		lift->SetPhysicsObject(new PhysicsObject(&lift->GetTransform(), lift->GetBoundingVolume()));

		lift->GetPhysicsObject()->SetInverseMass(0.001f);
		lift->GetPhysicsObject()->InitCubeInertia();
		lift->GetPhysicsObject()->FeelsGravity(0);
		//lift->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
		statemachines.push_back(lift);

		world->AddGameObject(lift);
	}
	return statemachines;
}

void TutorialGame::BallsGoBack(vector<GameObject*> balls)
{
	for (int i = 0; i < balls.size(); ++i)
	{
		if (balls[i]->GetTransform().GetPosition().z < 200)
		{
			balls[i]->GetTransform().SetPosition(ballPos1[i]);
		}
	}
}

void TutorialGame::BallsGoBack2(vector<GameObject*> balls)
{
	for (int i = 0; i < balls.size(); ++i)
	{
		if (balls[i]->GetTransform().GetPosition().z > -200)
		{
			balls[i]->GetTransform().SetPosition(ballpos2[i]);
		}
	}
}

void TutorialGame::PutStateMachine()
{
	std::vector<Vector3> statemachinePos = { Vector3( 120,10,50 ), Vector3(-150, 10, 10),
		Vector3(120, 10, -10), Vector3(-150, 10, -30) };
	statemachines = TestStateMachine(statemachinePos);

	std::vector<Vector3> statemachinePos2 = { Vector3(150, 10, 20), Vector3(-120, 20, -10),
		Vector3(150, 10, -30), Vector3(-120, 20, 30) };
	statemachinesSec = TestStateMachineSecond(statemachinePos2);

	

	std::vector<Vector3> threestatemachinePos = { Vector3(60, 5, -150), Vector3(20, 5, -120),
		Vector3(-20, 5, -150), Vector3(-60, 5, -120) };
	threeStates = ThreeStatesMachine(threestatemachinePos);


	std::vector<Vector3> threestatemachinePos2 = { Vector3(-65, 5, 150), Vector3(-20, 5, 120),
		Vector3(25, 5, 150), Vector3(65, 5, 120) };
	threeStatesSec = ThreeStatesMachineSecond(threestatemachinePos2);

	

	std::vector<Vector3> fourstatemachinePos = { Vector3(20, 15, 110), Vector3(-70, 15, -80)};
	fourStates = FourStatesMachine(fourstatemachinePos);
	

	std::vector<Vector3> fourstatemachinePos2 = { Vector3(-10, 15, 50), Vector3(60, 15, -70) };
	fourStatesSec = FourStatesMachineSecond(fourstatemachinePos2);

	

	std::vector<Vector3> specialstatemachinePos = { Vector3(200, 0, 300), Vector3(-200, 0, -170) };
	specialState = SpecialStatesMachine(fourstatemachinePos2);

	

	

	

	std::vector<Vector3> liftstatePos = { Vector3(50, 5 ,25), Vector3(-50, 5, -25) };
	liftState = LiftStateMachine(liftstatePos);

	std::vector<Vector3> thebeatPos = { Vector3(50, 20, 70) };
	beatState = BeatState(thebeatPos);


	std::vector<Vector3> ballPos = { Vector3(50, 40, 260),Vector3(25, 40, 240),Vector3(0, 40, 260),Vector3(-25, 40, 240),Vector3(-50, 40, 260) };
	ballPos1 = ballPos;
	balls = BallsState(ballPos);


	std::vector<Vector3> ballPos2 = { Vector3(50, 40, -260),Vector3(25, 40, -240),Vector3(0, 40, -260),Vector3(-25, 40, -240),Vector3(-50, 40, -260) };
	ballpos2 = ballPos2;
	balls1 = BallsState1(ballPos2);
	
}

void TutorialGame::UpdateStateMachine(float dt)
{
	if (!statemachines.empty()) {
		for (int i = 0; i < statemachines.size(); ++i) {
			statemachines.at(i)->Update(dt);
		}
	}



	if (!statemachinesSec.empty())
	{
		for (int i = 0; i < statemachinesSec.size(); ++i) {
			statemachinesSec.at(i)->Update(dt);
		}
	}


	if (!threeStates.empty())
	{
		for (int i = 0; i < threeStates.size(); ++i) {
			threeStates.at(i)->Update(dt);
		}
	}


	if (!threeStatesSec.empty())
	{
		for (int i = 0; i < threeStatesSec.size(); ++i) {
			threeStatesSec.at(i)->Update(dt);
		}
	}



	if (!fourStates.empty())
	{
		for (int i = 0; i < fourStates.size(); ++i) {
			fourStates.at(i)->Update(dt);
		}
	}



	if (!fourStatesSec.empty())
	{
		for (int i = 0; i < fourStatesSec.size(); ++i) {
			fourStatesSec.at(i)->Update(dt);
		}
	}



	if (!specialState.empty())
	{
		for (int i = 0; i < specialState.size(); ++i) {
			specialState.at(i)->Update(dt);
		}
	}



	if (!doorState.empty())
	{
		for (int i = 0; i < doorState.size(); ++i) {
			doorState.at(i)->Update(dt);
		}
	}



	if (!doorStateSec.empty())
	{
		for (int i = 0; i < doorStateSec.size(); ++i) {
			doorStateSec.at(i)->Update(dt);
		}
	}



	if (!liftState.empty())
	{
		for (int i = 0; i < liftState.size(); ++i) {
			liftState.at(i)->Update(dt);
		}
	}

	if (!beatState.empty())
	{
		for (int i = 0; i < beatState.size(); ++i) {
			beatState.at(i)->Update(dt);
		}
	}
}

double TutorialGame::GetCurrentCPU() {
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;

	return percent * 100;
}