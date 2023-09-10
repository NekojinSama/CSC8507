#include "NetworkedGame.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "RenderObject.h"

NetworkedGame::NetworkedGame(GameWorld* w, GameTechRenderer* r) : TutorialGame(w, r) {
	thisServer = nullptr;
	thisClient = nullptr;
	localPlayer = nullptr;
	currentObjective = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket  = 0.0f;
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
	serverObjects.clear();
	serverPlayers.clear();
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Client_ID, this);
	thisServer->RegisterPacketHandler(Client_Update, this);
	thisServer->RegisterPacketHandler(Client_Disconnected, this);
	thisServer->RegisterPacketHandler(State_Acknowledged, this);
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());

	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Client_ID, this);
	thisClient->RegisterPacketHandler(Start_Lobby, this);
	thisClient->RegisterPacketHandler(Full_Update, this);
	thisClient->RegisterPacketHandler(Delta_Update, this);
	thisClient->RegisterPacketHandler(End_Game, this);
}

void NetworkedGame::UpdateGame(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 60.0f;
	}

	if (thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
		CreateLobby();
	}

	if (state == 1) {
		if (thisClient) {
			for (auto& i : serverPlayers) {
				i.second->ClientUpdate(dt);
			}

			if (localPlayer) {
				Vector3 position = localPlayer->GetTransform().GetPosition();

				float distanceSqr = std::pow(localClientPosition.x - position.x, 2) +
					std::pow(localClientPosition.y - position.y, 2) +
					std::pow(localClientPosition.z - position.z, 2);

				if (!localPlayer->IsMoving() || (distanceSqr > 30 && distanceSqr <= 45)) {
					localPlayer->GetTransform().SetPosition(Vector3::Lerp(position, localClientPosition, dt));
				}
				else if (distanceSqr > 45) {
					localPlayer->GetTransform().SetPosition(localClientPosition);
				}
			}
		}
		else {
			for (auto& i : serverPlayers) {
				i.second->ServerUpdate(dt);
			}
		}
	}

	TutorialGame::UpdateGame(dt);

	if (timer <= 0 && thisServer) {
		winner = (scores[0] > scores[1]) ? 1 : ((scores[1] != scores[0]) ? 2 : 0);
		EndGamePacket packet = EndGamePacket(winner);
		thisServer->SendGlobalPacket(packet);
		state = 4;
	}
}

NetworkedPlayer* NetworkedGame::AddNetworkedPlayerToWorld(const Vector3& position, const int id, const int team) {
	float meshSize = 1.0f;
	float inverseMass = 0.9f;

	NetworkedPlayer* p = new NetworkedPlayer(world, this, id, team);

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

void NetworkedGame::CreateLobby() {
	StartLevel();

	std::vector<int> clientIDs = thisServer->GetConnectedClients();

	Vector3 objectivePosition = Vector3(10, 3, 10);
	std::map<int, Vector3> playerPositions;
	int teams[8];
	int aSize = 0;
	int bSize = 0;
	for (int i = 0; i < 8; i++) teams[i] = 0;
	for (auto& i : clientIDs) {
		Vector3 position = Vector3(i, 5, i);

		int team = 0;
		if (bSize < aSize) {
			team = 1;
			bSize++;
		}
		else {
			aSize++;
		}

		teams[i] = team;

		serverPlayers.emplace(i, AddNetworkedPlayerToWorld(position, i, team));
		playerPositions.emplace(i, position);
	}

	currentObjective = AddObjectiveToWorld(objectivePosition, Vector3(10, 3, 10), Vector3(1, 25, 1));
	serverObjects.push_back(currentObjective);

	thisServer->StartLobby(playerPositions, objectivePosition, teams);

	playerPositions.clear();
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer(dt);
}

void NetworkedGame::UpdateAsClient(float dt) {
	if (localPlayer != nullptr) {
		ClientPacket packet = ClientPacket(thisClient->GetClientID(),
											localPlayer->GetInputs(),
											localPlayer->GetTransform().GetOrientation(),
											localPlayer->GetItem(),
											localPlayer->GetCameraFwd(),
											localPlayer->GetEffect());
		thisClient->SendPacket(packet);
	}
	
	thisClient->UpdateClient();
}

void NetworkedGame::StartLevel() {
	state = 1;
	InitWorld();
	InitCamera();
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	if (thisServer) {
		switch (payload->type) {
		case Client_Update:
			MovePlayer(*payload);
			UpdateMasterState(*payload);
			SendServerUpdate(*payload);
			break;
		case State_Acknowledged:
			thisServer->AcknowledgeSnapshot(*payload);
			break;
		case Client_Disconnected:
			DisconnectPlayer(*payload);
			break;
		}
	}
	if (thisClient) {
		switch (payload->type) {
		case Client_ID:
			if (thisClient->GetClientID() == -1) {
				thisClient->SetClientID(((ClientID*)payload)->clientID);
			}
			break;
		case Start_Lobby:
			StartLobby(payload);
			break;
		case Full_Update:
			FullUpdate(*payload);
			break;
		case Delta_Update:
			DeltaUpdate(*payload);
			break;
		case End_Game:
			thisClient->LeaveLobby();
			state = 4;
			winner = ((EndGamePacket*)payload)->winningTeam;
			break;
		}
	}
}

void NetworkedGame::StartLobby(GamePacket* p) {
	StartLobbyPacket* packet = (StartLobbyPacket*)p;
	std::map<int, Vector3> playerPositions = packet->GetPlayerPositions();
	Vector3 objectivePosition = packet->objectivePosition;

	StartLevel();

	for (auto& i : playerPositions) {
		if (i.first == thisClient->GetClientID()) {
			localPlayer = AddPlayerToWorld(i.second, i.first, packet->teams[i.first]);
			localPlayer->SetRenderObject(nullptr);
		}
		else if (i.first != -1) {
			serverPlayers.emplace(i.first, AddNetworkedPlayerToWorld(i.second, i.first, packet->teams[i.first]));
		}
	}
	
	currentObjective = AddObjectiveToWorld(objectivePosition, Vector3(), Vector3(1, 1, 1));
}

void NetworkedGame::MovePlayer(GamePacket& packet) {
	ClientPacket clientPacket = (ClientPacket&)packet;
	if (serverPlayers.size() <= clientPacket.clientID) return;
	
	NetworkedPlayer* player = serverPlayers.at(clientPacket.clientID);
	player->GetTransform().SetOrientation(clientPacket.orientation);

	Ray isGround(player->GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision coll;
	bool onGround = false;

	if (world->Raycast(isGround, coll, true, player)) {
		onGround = (coll.rayDistance < 1.5f);
	}

	Vector3 direction = (Vector3(0, 0, -1) * clientPacket.keyStates[0])
		+ (Vector3(0, 0, 1) * clientPacket.keyStates[1])
		+ (Vector3(-1, 0, 0) * clientPacket.keyStates[2])
		+ (Vector3(1, 0, 0) * clientPacket.keyStates[3])
		+ (Vector3(0, 125, 0) * clientPacket.keyStates[4] * onGround);

	if (clientPacket.keyStates[7]) {
		player->SetSlide(false);
	}

	player->SetServerMovement(direction, clientPacket.effect);
}

int NetworkedGame::UpdateMasterState(GamePacket& packet) {
	ClientPacket clientPacket = (ClientPacket&)packet;
	if (serverPlayers.size() <= clientPacket.clientID) return -1;
	int clientID = clientPacket.clientID;

	thisServer->GetMasterState()->positions[clientID] = serverPlayers.at(clientID)->GetTransform().GetPosition();
	thisServer->GetMasterState()->orientations[clientID] = serverPlayers.at(clientID)->GetTransform().GetOrientation();
	thisServer->GetMasterState()->timer = timer;
	thisServer->GetMasterState()->aScore = scores[0];
	thisServer->GetMasterState()->bScore = scores[1];
	if (clientPacket.keyStates[5]) {
		thisServer->GetMasterState()->paint[clientID] = clientPacket.paint;
	}
	else if (clientPacket.keyStates[6]) {
		thisServer->GetMasterState()->paint[clientID] = 0;
	}
	else {
		thisServer->GetMasterState()->paint[clientID] = -1;
	}
	thisServer->GetMasterState()->cameraFwd[clientID] = clientPacket.cameraFwd;

	thisServer->GetMasterState()->objectivePosition = currentObjective->GetTransform().GetPosition();

	thisServer->AddSnapshot(clientID);

	return clientID;
}

void NetworkedGame::SendServerUpdate(GamePacket& packet) {
	ClientPacket clientPacket = (ClientPacket&)packet;
	if (serverPlayers.size() <= clientPacket.clientID) return;

	NetworkState* lastValidState = thisServer->GetLastValidSnapshot(clientPacket.clientID);
	NetworkState* currentState = thisServer->GetLatestSnapshot(clientPacket.clientID);

	Vector3 positions[8];
	Quaternion orientations[8];
	int painting[8];
	Vector3 cameraFwd[8];

	std::fill(positions, positions + 8, Vector3());
	std::fill(orientations, orientations + 8, Quaternion());
	std::fill(painting, painting + 8, -1);
	std::fill(cameraFwd, cameraFwd + 8, Vector3());

	for (int i = 0; i < serverPlayers.size(); i++) {
		positions[i] = currentState->positions.at(i);
		orientations[i] = currentState->orientations.at(i);
		painting[i] = currentState->paint.at(i);
		cameraFwd[i] = currentState->cameraFwd.at(i);
	}

	FullPacket serverPacket = FullPacket(currentState->snapshotID, positions, orientations, currentState->timer,
											currentState->aScore, currentState->bScore, painting, cameraFwd,
											currentState->objectivePosition);

	thisServer->SendPacket(clientPacket.clientID, serverPacket);
}

void NetworkedGame::FullUpdate(GamePacket& payload) {
	FullPacket packet = (FullPacket&)payload;
	UpdateTimer(packet.time);
	UpdateScore(packet.objectivePosition, packet.aScore, packet.bScore);
	UpdateTransforms(packet.positions, packet.orientations);
	PaintWorld(packet.painting, packet.cameraFwd);
	AcknowledgeGameState(packet.stateID);
}

void NetworkedGame::DeltaUpdate(GamePacket& payload) {
	//DeltaPacket packet = (DeltaPacket&)payload;
	//UpdateTimer(packet.time);
	//
	//if (packet.changes[0]) {
	//	UpdateTransforms(packet.positions, packet.orientations);
	//}

	//if (packet.changes[1]) PaintWorld(packet.painting, packet.cameraFwd);

	//if (packet.changes[2]) UpdateScore(packet.objectivePosition, packet.aScore, packet.bScore);
	//
	//AcknowledgeGameState(packet.stateID);
}

void NetworkedGame::UpdateTimer(float serverTime) {
	timer = (serverTime - 0.5f > timer) || (serverTime + 0.5f < timer) ? serverTime : timer;
}

void NetworkedGame::UpdateScore(Vector3 objPos, int scoreA, int scoreB) {
	scores[0] = scoreA;
	scores[1] = scoreB;
	currentObjective->GetTransform().SetPosition(objPos);
}

void NetworkedGame::UpdateTransforms(Vector3 positions[8], Quaternion orientations[8]) {
	for (auto& i : serverPlayers) {
		i.second->SetServerPosition(positions[i.first]);
		i.second->SetServerOrientation(orientations[i.first]);
	}
	localClientPosition = positions[localPlayer->GetID()];
}

void NetworkedGame::PaintWorld(int paintType[8], Vector3 directions[8]) {
	for (auto& i : serverPlayers) {
		if (paintType[i.first] != -1) {
			Vector3 colour = Vector3();
			switch (paintType[i.first]) {
			case 1:
				colour = Vector3(1, 0, 0);
				break;
			case 2:
				colour = Vector3(0, 0, 1);
				break;
			case 3:
				colour = Vector3(0, 1, 0);
				break;
			}
			ApplyPaint(colour, (i.second)->GetTransform().GetPosition(), directions[i.first], i.second);
		}
	}
}

void NetworkedGame::AcknowledgeGameState(int stateID) {
	AcknowledgePacket sendPacket = AcknowledgePacket(thisClient->GetClientID(), stateID);
	thisClient->SendPacket(sendPacket);
}

void NetworkedGame::DisconnectPlayer(GamePacket& payload) {
	DisconnectedPacket packet = (DisconnectedPacket&)payload;
	serverPlayers.erase(packet.clientID);
}