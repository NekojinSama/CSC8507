#pragma once

#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "SoundDevice.h"
#include "SoundBuffer.h"
#include "SoundSource.h"
#include "SoundSystem.h"
#include "StateGameObject.h"
#include <NavigationMesh.h>
#include "Player.h"
#include "Objective.h"
#include "AssetManager.h"
#include "ConfigOptions.h"

// Refactor me
#include "StateGameObject.h"

namespace NCL {
	namespace CSC8503 {
		class LevelBuilder;
		class AIEntity;
		class HUD;

		class TutorialGame {
		public:
			TutorialGame(GameWorld* w, GameTechRenderer* r);
			~TutorialGame();

			virtual void UpdateGame(float dt);

			void StartSolo();

			double GetCurrentCPU();

			void ApplyPaint(Vector3 colour, Vector3 position, Vector3 direction, GameObject* ignore);

			const bool HasStarted() const {
				return state == 0;
			}

			const bool IsPaused() const {
				return state == 2;
			}

			const bool IsFinished() const {
				return state == 4;
			}

			const GameTechRenderer* GetRenderer() const {
				return renderer;
			}

			void Unpause(bool endGame) {
				state = endGame ? -1 : 1;
			}

			const float GetTimer() const {
				return timer;
			}

			const int GetScore(int team) const {
				return scores[team];
			}

			const float GetFPS() const {
				return averageFrameRate;
			}

			const Player* GetLocalPlayer() const {
				return localPlayer;
			}

			void SetDetectPaintTime(double timeSpent) { detectPaintTimer = timeSpent; }
			void SetApplyPaintTime(double timeSpent) { applyPaintTimer = timeSpent; }
			void SetAssetsTimer(double timeSpent) { assetsTimer = timeSpent; }

			float averageFrameRate;
			std::size_t RAMUsage;
			std::size_t VirtualMemUsage;
			double CPUUsage;
			double applyPaintTimer = 0;
			double detectPaintTimer = 0;
			double assetsTimer = 0;
			double physicsTimer = 0;

			int GetWinner() { return winner; }

		protected:
			void InitCamera();
			void InitWorld();

			void UpdateAI(float dt);

			GameObject* AddBasicLevel(const Vector3& position);
			Objective* AddObjectiveToWorld(const Vector3& position, Vector3 orientation, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddFlagpoleToWorld(const Vector3& position);//dral
			GameObject* AddTeamFlagToWorld();//dral
			void RaisingFlag(float dt);//dral
			GameObject* AddAnoFlagPoleToWorld(const Vector3& position);
			GameObject* AddAnoTeamFlagToWorld();
			void RaisingAnoFlag(float dt);
			Player* AddPlayerToWorld(const Vector3& position, const int id, const int team);


			/*bounds = The area around position that the AI is allowed to wander*/
			AIEntity* AddAIToWorld(const Vector3& position, Vector2 bounds, vector<Player*> allPlayers);
			void AddAllAI(vector<Player*> allPlayers);

			NavigationMesh* navMesh;

			Player*		localPlayer;
			Objective*	currentObjective;
			vector<AIEntity*> aiEntities;

			MEMORYSTATUSEX memoryInfo;


			// Refactor me
			std::vector<StateGameObject*>  statemachines;//dral
			std::vector<StateGameObject*>  TestStateMachine(const std::vector<Vector3>& positions);

			

			std::vector<StateGameObject2*> statemachinesSec;
			std::vector<StateGameObject2*> TestStateMachineSecond(const std::vector<Vector3>& positions);

			
			

			std::vector<TestStateThree*> threeStates;
			std::vector<TestStateThree*> ThreeStatesMachine(const std::vector<Vector3>& positions);


			

			std::vector<TestStateThree2*> threeStatesSec;
			std::vector<TestStateThree2*> ThreeStatesMachineSecond(const std::vector<Vector3>& positions);

			

			std::vector<TestStateFour*> fourStates;
			std::vector<TestStateFour*> FourStatesMachine(const std::vector<Vector3>& positions);

			

			std::vector<TestStateFour2*> fourStatesSec;
			std::vector<TestStateFour2*> FourStatesMachineSecond(const std::vector<Vector3>& positions);


			std::vector<SpecialState*> specialState;
			std::vector<SpecialState*> SpecialStatesMachine(const std::vector<Vector3>& positions);

		

			std::vector<DoorState*> doorState;
			std::vector<DoorState*> DoorStatesMachine(const std::vector<Vector3>& positions);

			

			std::vector<DoorState2*> doorStateSec;
			std::vector<DoorState2*> DoorStatesMachineSecond(const std::vector<Vector3>& positions);

			
			std::vector<TheBeat*> beatState;
			std::vector<TheBeat*> BeatState(const std::vector<Vector3>& positions);
			

			GameObject* MovingPool(const Vector3& position);
			GameObject* MovingPool2(const Vector3& position);

			GameObject* MovingPoolSecond(const Vector3& position);
			GameObject* MovingPoolSecond2(const Vector3& position);

			std::vector<TheLift*> liftState;
			std::vector<TheLift*> LiftStateMachine(const std::vector<Vector3>& positions);

			std::vector<GameObject*> balls;
			std::vector<GameObject*> BallsState(const std::vector<Vector3>& positions);

			std::vector<GameObject*> balls1;
			std::vector<GameObject*> BallsState1(const std::vector<Vector3>& positions);
			

			GameObject* BindFlag(GameObject* pole);
			GameObject* flagT = nullptr;
			void RandomFlag();

			
			void PutStateMachine();
			void UpdateStateMachine(float dt);
			void BallsGoBack(vector<GameObject*> balls);
			vector<Vector3> ballPos1;
			vector<GameObject*> ballsta;
			void BallsGoBack2(vector<GameObject*> balls);
			vector<Vector3> ballpos2;
			vector<GameObject*> ballsta2;
			


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;
			LevelBuilder* levelLoader;
			HUD* hud;

			bool canPause;

			float timer;
			int winner;

			float frameTimes[10];
			int newestTime;

			int scores[2];
			int state;

			// Refactor me
			TextureBase* lavaTex = nullptr;
			TextureBase* lavaTex2 = nullptr;
			ShaderBase* flagShader = nullptr;
			ShaderBase* edgeShader = nullptr;
			ShaderBase* lavaShader = nullptr;
			ShaderBase* gateShader = nullptr;
			GLuint bufferFBO;//dral
			
			
			GameObject* testCube;//dral
			GameObject* flagpole = nullptr;//dral
			GameObject* teamflag = nullptr;//dral
			GameObject* anoflagpole = nullptr;//dral
			GameObject* anoteamflag = nullptr;//dral
			Objective* movingfl = nullptr;
			float rise = 0;//dral

		private:
			GameObject* sphereCheckObj;
		};
	}
}
 
