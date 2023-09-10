#pragma once
#include "imgui/imgui.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame;
		class GameTechRenderer;
		class Player;

		struct Values{
			ImVec4 color;
			std::string buf;
			float progress;
		};

		class HUD {
		public:
			HUD(TutorialGame* tutGame);
			~HUD() {}

			void GameHUD(GameTechRenderer* r);
			void WeaponHUD(GameTechRenderer* r, Player* p);
			void CoopGameWeaponHUD(GameTechRenderer* r);
			void setCameraCount(int i) { numCameras = i; };
			void DebugHUD(GameTechRenderer* r);
			Values GetPlayerInfo(const Player* p);

			void setPlayerList(std::list<Player*> p) { players = p; }
			std::list<Player*> getplayerList() { return players; }

		private:
			TutorialGame* game;
			GameTechRenderer* renderer;
			int numCameras;
			std::list<Player*> players;
		};
	}
}

