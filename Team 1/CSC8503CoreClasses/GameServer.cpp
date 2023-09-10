#include "GameServer.h"
#include "GameWorld.h"
#include "./enet/enet.h"

#include <NetworkObject.h>

using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients)	{
	port		= onPort;
	clientMax	= maxClients;
	clientCount = 0;
	netHandle	= nullptr;
	Initialise();
}

GameServer::~GameServer()	{
	connectedClients.clear();
	delete snapshots;
	delete dummyState;
	delete masterState;
	Shutdown();
}

void GameServer::Shutdown() {
	SendGlobalPacket(BasicNetworkMessages::Shutdown);
	enet_host_destroy(netHandle);
	netHandle = nullptr;
}

bool GameServer::Initialise() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	
	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);
	
	if (!netHandle) {
		std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
		return false;
	}
	return true;
}

bool GameServer::SendGlobalPacket(int msgID) {
	GamePacket packet;
	packet.type = msgID;
	return SendGlobalPacket(packet);
}

bool GameServer::SendGlobalPacket(GamePacket& packet) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_host_broadcast(netHandle, 0, dataPacket);
	return true;
}

void GameServer::SendPacket(int clientID, GamePacket& packet) {
	ENetPacket* dataPacket = enet_packet_create(&packet, packet.GetTotalSize(), 0);
	enet_peer_send(connectedClients.at(clientID), 0, dataPacket);
}

void GameServer::UpdateServer(float dt) {
	if (!netHandle) { return; }
	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0) {
		int type = event.type;
		ENetPeer * p = event.peer;
		int peer = p->incomingPeerID;
		
		if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT) {
			int clientID = connectedClients.size();
			connectedClients.emplace(clientID, p);
			ClientID packet = ClientID(clientID);
			SendGlobalPacket(packet);
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT) {

		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE) {
			GamePacket* packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet, peer);
		}
		enet_packet_destroy(event.packet);
	}
}

void GameServer::SetGameWorld(GameWorld &g) {
	gameWorld = &g;
}

void GameServer::StartLobby(std::map<int, Vector3> playerPositions, Vector3 objectivePosition, int teams[8]) {
	StartLobbyPacket packet = StartLobbyPacket(playerPositions, objectivePosition, teams);
	SendGlobalPacket(packet);

	dummyState = new NetworkState(connectedClients.size());
	masterState = new NetworkState(connectedClients.size());

	for (int i = 0; i < 8; i++) {
		latestSnapshot[i] = 0;
		for (int j = 0; j < 60; j++) {
			snapshots[i][j] = dummyState;
		}
	}
}

void GameServer::AddSnapshot(int clientID) {
	latestSnapshot[clientID] = latestSnapshot[clientID] + 1 >= 60 ? latestSnapshot[clientID] = 0 : latestSnapshot[clientID] + 1;
	snapshots[clientID][latestSnapshot[clientID]] = masterState;
	snapshots[clientID][latestSnapshot[clientID]]->snapshotID = latestSnapshot[clientID];
}

NetworkState* GameServer::GetLastValidSnapshot(int clientID) {
	NetworkState* validState = dummyState;
	
	for (int i = latestSnapshot[clientID] - 1; i != latestSnapshot[clientID]; i--) {
		if (i < 0) {
			i = 59;
		}
		
		if (snapshots[clientID][i]->isAcknowledged) {
			return snapshots[clientID][i];
		}
	}
	return validState;
}

void GameServer::AcknowledgeSnapshot(GamePacket& payload) {
	AcknowledgePacket packet = (AcknowledgePacket&)payload;
	snapshots[packet.clientID][packet.stateID]->isAcknowledged = true;
}