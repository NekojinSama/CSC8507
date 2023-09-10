#pragma once
#include "NetworkBase.h"

#include <Maths.h>
#include "NetworkState.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld &g);

			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);

			void SendPacket(int clientID, GamePacket& packet);

			void StartLobby(std::map<int, Vector3> playerPositions, Vector3 objectivePosition, int teams[8]);

			virtual void UpdateServer(float dt);

			std::vector<int> GetConnectedClients() {
				std::vector<int> tmp;
				for (auto& i : connectedClients) {
					tmp.push_back(i.first);
				}
				return tmp;
			}

			NetworkState* GetMasterState() {
				return masterState;
			}

			void AddSnapshot(int clientID);

			NetworkState* GetLatestSnapshot(int clientID) {
				return snapshots[clientID][latestSnapshot[clientID]];
			}

			NetworkState* GetLastValidSnapshot(int clientID);

			void AcknowledgeSnapshot(GamePacket& packet);

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld*	gameWorld;

			int incomingDataRate;
			int outgoingDataRate;

			std::map<int, _ENetPeer*> connectedClients;
			NetworkState* snapshots[8][60];
			int latestSnapshot[8];
			NetworkState* dummyState;
			NetworkState* masterState;
		};
	}
}
