#pragma once
#include "TutorialGame.h"

namespace NCL {
	namespace CSC8503 {
		class CoopGame : public TutorialGame {
		public:
			CoopGame(GameWorld* w, GameTechRenderer* r);
			~CoopGame();

			//Player* AddPlayerToWorld(Vector3 position, int team, bool controller);

			void StartCoop(int numPlayers);
			void UpdateGame(float dt) override;
			void InitCamera(int numPlayers);

			std::list<Player*> GetPlayers() { return players; }

		protected:
			Player* AddPlayerToWorld(const Vector3& position, const int id, const int controller, const int team);
			std::list<Player*> players;
		};
	}
}