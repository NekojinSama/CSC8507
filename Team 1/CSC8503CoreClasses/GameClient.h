#pragma once
#include "NetworkBase.h"
#include <stdint.h>
#include <thread>
#include <atomic>

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class GameClient : public NetworkBase {
		public:
			GameClient();
			~GameClient();

			bool Connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, int portNum);

			void Disconnect();
			void LeaveLobby();

			void SendPacket(GamePacket&  payload);

			void UpdateClient();

			int GetClientID() {
				return clientID;
			}

			void SetClientID(int ID) {
				clientID = ID;
			}
		protected:	
			_ENetPeer*	netPeer;

			int clientID = -1;
		};
	}
}

