#pragma once

namespace NCL {
	namespace CSC8503 {
		class TutorialGame;
		class NetworkedGame;
		class CoopGame;
		class GameWorld;
		class GameTechRenderer;
		class MainMenu {
		public:
			MainMenu(GameWorld* world);
			~MainMenu();

			void UpdateMainMenu();
			void UpdatePauseMenu();
			void UpdateEndScreen();
			void UpdateGame(float dt);

			bool isQuit;
			bool inMenu;
			bool isPaused;

			NetworkedGame* netGame;
			CoopGame* coopGame;
			TutorialGame* singleGame;
			GameWorld* world;
			GameTechRenderer* renderer;

		private:
			void StartSinglePlayer(GameWorld* world, GameTechRenderer* renderer);
			void StartServer(GameWorld* world, GameTechRenderer* renderer);
			void StartClient(GameWorld* world, GameTechRenderer* renderer);
			void StartCoopGame(GameWorld* world, GameTechRenderer* renderer);

			void BackToMainMenu();
		};
	}
}
