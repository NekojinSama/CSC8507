#pragma once
#include "GameTechRenderer.h"
#include "PhysicsObject.h"
#include "Assets.h"
#include "SoundBuffer.h"
#include "Gamepad.h"


#include "SoundDevice.h"
#include "SoundsManager.h"
#include "SoundSystem.h"
#include "Sounds.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class TutorialGame;
		class Player : public GameObject {
		public:
			Player(GameWorld* w, TutorialGame* g, int id, int t, Camera* cam, bool hasController = false);
			Player(GameWorld* w, TutorialGame* g, int team, int playerNum, int hasController);
			~Player();

			virtual void Update(float dt);

			const bool* GetInputs() const {
				return keyPress;
			}

			float GetSpeed() const {
				return speed;
			}

			int GetTeam() const {
				return team;
			}

			int GetID() const {
				return ID;
			}

			int GetItem() const {
				return currentItem;
			}

			Vector3 GetCameraFwd() const {
				return cameraFwd;
			}

			int GetEffect() const {
				return effect;
			}

			bool IsMoving() const {
				for (int i = 0; i < 7; i++) {
					if (keyPress[i]) return true;
				}
				return false;
			}

			const int GetCurrentItem() const {
				return currentItem;
			}

			const bool IsBlind() const {
				return isBlind;
			}

			void SetSlide(bool value) {
				canSlide = value;
				slideTime = MAX_SLIDE_TIME;
			}

			const float GetPaintTimer() const { return paintTimer; }

			float blindTimer = 5.0f;

		protected:
			void PlayerCamera(float dt);
			void PlayerMovement(float dt);
			void ApplyMovement(const float dt, const Vector3 direction, const int effect);
			void PlayerActions(float dt);

			void DetectPaint();

			enum Item {
				Clear,
				Speed,
				Slow,
				Bounce,
				Confusion,
				Splat,
				Last
			};


			bool isBlind;

			float slideTime = 0.0f;
			const float MAX_SLIDE_TIME = 0.75f;
			bool canSlide = true;
			bool fiveSecondsHasPassed = true;
			float fiveSeconds = 5.0f;

			float paintTimer;
			float releasedTimer;
			const float MAX_PAINT_TIME = 10.0f;
			const float MAX_RELEASE_TIME = 5.0f;

			int currentItem;
			const float CAMERA_Y_OFFSET = 1.0f;

			Camera* camera;
			GameWorld* world;
			TutorialGame* game;

			Controller* controller;

			float yaw;
			float pitch;

			bool onGround = false;
			bool keyPress[8];

			const float speed = 10;

			int buffer[1];
			
			GLuint SSBO;
			GLvoid* ssboPtr;

			int team;
			int ID;
			int effect;

			Vector3 cameraFwd;

			Sounds* walkAudio;
			SoundSource* walkAud;

			Sounds* jumpAudio;
			SoundSource* jumpAud;

			Sounds* paintAudio;
			SoundSource* paintAud;

			Sounds* cleanAudio;
			SoundSource* cleanAud;
		};
	}
}