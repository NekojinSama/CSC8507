#pragma once

#include "Player.h"
#include "Quaternion.h"

namespace NCL {
	namespace CSC8503 {

		class NetworkedPlayer : public Player {
		public:
			NetworkedPlayer(GameWorld* w, TutorialGame* g, int ID, int team);
			~NetworkedPlayer();

			void ClientUpdate(float dt);
			void ServerUpdate(float dt);

			void SetServerPosition(Vector3 pos) {
				serverPosition = pos;
			}

			void SetServerOrientation(Quaternion ori) {
				serverOrientation = ori;
			}

			void SetServerMovement(Vector3 direction, int effect) {
				serverDirection = direction;
				serverEffect = effect;
			}

		protected:
			Vector3 serverPosition;
			Quaternion serverOrientation;
			Vector3 serverDirection;
			int serverEffect;
		};

	}
}