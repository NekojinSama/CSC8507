#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"
#include "NetworkObject.h"
#include "NetworkedPlayer.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame(GameWorld* w, GameTechRenderer* r);
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void StartLobby(GamePacket* packet);

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void CreateLobby();

			void FullUpdate(GamePacket& packet);
			void DeltaUpdate(GamePacket& packet);

			void UpdateTimer(float packetTime);
			void UpdateScore(Vector3 objPos, int aScore, int bScore);
			void UpdateTransforms(Vector3 positions[8], Quaternion orientations[8]);
			void PaintWorld(int paintType[8], Vector3 directions[8]);
			void AcknowledgeGameState(int stateID);

			void MovePlayer(GamePacket& packet);
			int UpdateMasterState(GamePacket& packet);

			void SendServerUpdate(GamePacket& packet);

			void DisconnectPlayer(GamePacket& payload);

			NetworkedPlayer* AddNetworkedPlayerToWorld(const Vector3& position, const int id, const int team);

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;

			std::vector<GameObject*> serverObjects;

			std::map<int, NetworkedPlayer*> serverPlayers;

			Vector3 localClientPosition;
		};
	}
}

