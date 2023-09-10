#pragma once
#include "GameObject.h"
#include "PhysicsObject.h"
#include <NavigationMesh.h>
#include <State.h>
#include <StateMachine.h>
#include <StateTransition.h>
#include <random>
#include "Player.h"
#include "RenderObject.h"
#include "TutorialGame.h"

namespace NCL {
	namespace CSC8503 {
		class AIEntity : public GameObject{
		public:
			AIEntity(Vector3 position, NavigationMesh* mesh, Vector2 bound, vector<Player*> allPlayers, GameWorld* w, TutorialGame* g);
			~AIEntity() {
				//delete stateMachine;
			}

			void Start();
			void Update(float dt);

		protected:
			void Wander();
			void AttackPlayer();

			bool CheckCanSeePlayer();
			bool RaycastToPlayer(Player* player, Vector3 playerPos, Vector3 aiPos);
			bool WithinBounds(Vector3 position);
			void Respawn();
			Vector3 ChoosePosition();
			void GetPathToDest(Vector3 destination);
			float RandBetween(float a, float b);

			bool beginAttack = false;
			bool returnToWander = false;
			
			int currentPosIndex = 0;
			const float movementSpeed = 5;
			
			std::vector<Vector3> path;
			Vector3 homePos;
			Vector2 bounds;
			Vector3 dest = Vector3(0, 0, 0);
			Vector3 nextPos = Vector3(0, 0, 0);

			NavigationMesh* navMesh = nullptr;

			StateMachine* stateMachine;

			TutorialGame* game;
			GameWorld* world;
			vector<Player*> players;
			Player* targetedPlayer = nullptr;
		};
	}
}

