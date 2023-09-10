#pragma once
#include "GameObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"

#define ARRAY_SIZE(arr) (sizeof(*arr) / (sizeof(arr[0])))

namespace NCL::CSC8503 {
	class GameObject;

	struct ClientPacket : public GamePacket {
		int	clientID;
		int paint;
		int effect;
		bool keyStates[8];
		Quaternion orientation;
		Vector3 cameraFwd;
		

		ClientPacket(const int id, const bool states[8], const Quaternion ori, const int p, const Vector3 camFwd, const int e) {
			size = sizeof(ClientPacket);
			type = Client_Update;
			clientID = id;
			std::copy(states, states + 8, keyStates);
			orientation = ori;
			paint = p;
			cameraFwd = camFwd;
			effect = e;
		}
	};

	struct FullPacket : public GamePacket {
		int	stateID;
		Vector3 positions[8];
		Quaternion orientations[8];
		int painting[8];
		float time;
		int aScore, bScore;
		Vector3 cameraFwd[8];
		Vector3 objectivePosition;

		FullPacket(int gameStateID, Vector3 pos[8], Quaternion ori[8], float t, int scoreA, int scoreB, int pnt[8], Vector3 camFwd[8], Vector3 objPos) {
			size = sizeof(FullPacket);
			type = Full_Update;
			stateID = gameStateID;
			std::copy(pos, pos + 8, positions);
			std::copy(ori, ori + 8, orientations);
			std::copy(pnt, pnt + 8, painting);
			std::copy(camFwd, camFwd + 8, cameraFwd);
			time = t;
			aScore = scoreA;
			bScore = scoreB;
			objectivePosition = objPos;
		}
	};

	/*
	struct DeltaPacket : public GamePacket {
		int	stateID;
		int time;
		bool changes[3];

		Vector3 positions[8];
		Quaternion orientations[8];

		int painting[8];
		Vector3 cameraFwd[8];

		int aScore, bScore;
		Vector3 objectivePosition;

		DeltaPacket(int gameStateID, int time_) {
			type = Delta_Update;
			stateID = gameStateID;
			for (int i = 0; i < 3; i++) {
				changes[i] = false;
			}
			time = time_;
			//size = sizeof(GamePacket) + sizeof(time) + sizeof(changes);
			size = sizeof(DeltaPacket);
		}

		void AddPositions(Vector3 pos[8], Quaternion ori[8]) {
			changes[0] = true;
			std::copy(pos, pos + 8, positions);
			std::copy(ori, ori + 8, orientations);
			//size += (sizeof(positions) + sizeof(orientations));
		}

		void AddPaint(int pnt[8], Vector3 camFwd[8]) {
			changes[1] = true;
			std::copy(pnt, pnt + 8, painting);
			std::copy(camFwd, camFwd + 8, cameraFwd);
			//size += (sizeof(painting) + sizeof(cameraFwd));
		}

		void AddScore(Vector3 objPos, int aScore_, int bScore_) {
			changes[2] = true;
			objectivePosition = objPos;
			aScore = aScore_;
			bScore = bScore_;
			//size += (sizeof(aScore) + sizeof(bScore));
		}
	};
	*/

	struct AcknowledgePacket : public GamePacket {
		int stateID;
		int clientID;

		AcknowledgePacket(int client, int state) {
			size = sizeof(AcknowledgePacket);
			type = State_Acknowledged;
			stateID = state;
			clientID = client;
		}
	};

	struct ClientID : GamePacket {
		int clientID;

		ClientID(int id) {
			clientID = id;
			size = sizeof(ClientID);
			type = Client_ID;
		}
	};

	struct StartLobbyPacket : public GamePacket {
		int IDs[8];
		int teams[8];
		Vector3 positions[8];
		Vector3 objectivePosition;

		StartLobbyPacket(std::map<int, Vector3> playerPositions, Vector3 vec, int team[8]) {
			size = sizeof(StartLobbyPacket);
			type = Start_Lobby;
			objectivePosition = vec;

			std::copy(team, team + 8, teams);

			int i = 0;
			for (auto pair : playerPositions) {
				IDs[i] = pair.first;
				positions[i] = pair.second;
				i++;
			}
			while (i < 8) {
				IDs[i] = -1;
				i++;
			}
		}

		std::map<int, Vector3> GetPlayerPositions() {
			std::map<int, Vector3> playerPositions;
			for (int i = 0; i < 8; i++) {
				playerPositions.emplace(IDs[i], positions[i]);
			}
			
			return playerPositions;
		}
	};

	struct EndGamePacket : GamePacket {
		int winningTeam;
		
		EndGamePacket(int winner) {
			type = End_Game;
			size = sizeof(EndGamePacket);
			winningTeam = winner;
		}
	};

	struct DisconnectedPacket : GamePacket {
		int clientID;

		DisconnectedPacket(int id) {
			clientID = id;
			size = sizeof(DisconnectedPacket);
			type = Client_Disconnected;
		}
	};

	class NetworkObject		{
	public:
		NetworkObject(GameObject& o, int id);
		virtual ~NetworkObject();

	protected:
		GameObject& object;

		int networkID;
	};
}