#include "AIEntity.h"

using namespace NCL;
using namespace CSC8503;

AIEntity::AIEntity(Vector3 position, NavigationMesh* mesh, Vector2 bound, vector<Player*> allPlayers, GameWorld* w, TutorialGame* g){
	homePos = position;
	navMesh = mesh;
	bounds = bound;
	players = allPlayers;
	world = w;
	game = g;
}

void AIEntity::Start() {
	stateMachine = new StateMachine();
	dest = ChoosePosition();
	GetPathToDest(dest);

	State* stateA = new State([&](float dt)->void {	this->Wander();			});
	State* stateB = new State([&](float dt)->void {	this->AttackPlayer();	});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool {	return beginAttack;		}));
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool {	return returnToWander;	}));
}

void AIEntity::Update(float dt){
	stateMachine->Update(dt);
}

void AIEntity::Wander(){
	beginAttack = CheckCanSeePlayer();
	GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	Vector3 aiPos = GetTransform().GetPosition();
	aiPos.y = nextPos.y;
	float distToNextPos = (nextPos - aiPos).Length();

	if (distToNextPos <= 1.0f) {
		if (nextPos == dest) {
			dest = ChoosePosition();
			GetPathToDest(dest);
		}
		else {
			nextPos = path[++currentPosIndex];
		}
	}
	else {
		Vector3 nextPosDirection = (nextPos - aiPos).Normalised();
		GetPhysicsObject()->AddForce(nextPosDirection * movementSpeed);
	}
}

void AIEntity::AttackPlayer(){
	returnToWander = !CheckCanSeePlayer();
	GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	if (targetedPlayer) {
		Vector3 aiPos = GetTransform().GetPosition();
		Vector3 playerPos = targetedPlayer->GetTransform().GetPosition();
		playerPos.y -= targetedPlayer->GetTransform().GetScale().y / 2; //Shoot at player's feet

		Vector3 rayDir = (playerPos - aiPos).Normalised();

		game->ApplyPaint(Vector3(0, 1, 1), GetTransform().GetPosition(), rayDir, this);
	}
}

bool AIEntity::CheckCanSeePlayer(){
	Player* closestPlayerCanSee = nullptr;
	float closestDistance = FLT_MAX;
	Vector3 aiPos = GetTransform().GetPosition();

	for (int i = 0; i < players.size(); i++) {

		Vector3 playerPos = players[i]->GetTransform().GetPosition();
		float distance = (playerPos - aiPos).Length();

		if (distance < closestDistance) {
			closestDistance = distance;
			closestPlayerCanSee = RaycastToPlayer(players[i], playerPos, aiPos) ? players[i] : closestPlayerCanSee;
		}
	}

	targetedPlayer = closestPlayerCanSee;
	return closestPlayerCanSee;
}

bool AIEntity::RaycastToPlayer(Player* player, Vector3 playerPos, Vector3 aiPos) {
	Vector3 rayDir = playerPos - aiPos;

	if (!WithinBounds(playerPos))
		return false;

	Ray ray = Ray(aiPos, rayDir.Normalised());
	RayCollision closestCollision;
	if (world->Raycast(ray, closestCollision, true, this)) {
		GameObject* collidedObject = (GameObject*)closestCollision.node;
		if (collidedObject == player)
			return true;
	}
	return false;
}

bool AIEntity::WithinBounds(Vector3 position){
	Vector3 offsetFromSpawn = (position - homePos);
	bool outOfBoundsX = abs(offsetFromSpawn.x) > bounds.x;
	bool outOfBoundZ = abs(offsetFromSpawn.z) > bounds.y;

	return !(outOfBoundsX || outOfBoundZ);
}

void AIEntity::Respawn(){
	GetTransform().SetPosition(homePos);
	dest = ChoosePosition();
	GetPathToDest(dest);
	returnToWander = true;
}

Vector3 AIEntity::ChoosePosition(){
	Vector3 aiPos = GetTransform().GetPosition();

	float destX = homePos.x + RandBetween(-(bounds.x / 2), bounds.x / 2);
	float destZ = homePos.z + RandBetween(-(bounds.y / 2), bounds.y / 2);


	return Vector3(destX, homePos.y, destZ);
}

void AIEntity::GetPathToDest(Vector3 destination){
	Vector3 aiPos = GetTransform().GetPosition();

	NavigationPath outPath;
	bool found = navMesh->FindPath(aiPos, dest, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		path.push_back(pos);
	}

	currentPosIndex = 0;
	nextPos = path[currentPosIndex];
}

float AIEntity::RandBetween(float a, float b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(a, b);

	return distr(gen);
}
