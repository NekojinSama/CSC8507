#include "NetworkedPlayer.h"

NetworkedPlayer::NetworkedPlayer(GameWorld* w, TutorialGame* g, int ID, int team) : Player(w, g, ID, team, w->GetMainCamera()) {
	serverPosition = Vector3();
	serverOrientation = Quaternion();
	serverDirection = Vector3();
	serverEffect = 0;
}

NetworkedPlayer::~NetworkedPlayer() {

}

void NetworkedPlayer::ClientUpdate(float dt) {
	Vector3 position = GetTransform().GetPosition();
	Quaternion orientation = GetTransform().GetOrientation();

	if (position != serverPosition) {
		GetTransform().SetPosition(Vector3::Lerp(position, serverPosition, dt));
	}
	if (orientation != serverOrientation) {
		GetTransform().SetOrientation(serverOrientation);
	}
}

void NetworkedPlayer::ServerUpdate(float dt) {
	if (!canSlide) {
		if (slideTime > 0) {
			slideTime -= dt;
			serverDirection *= 2.0f;
		}
		else if (slideTime <= 0) {
			canSlide = true;
		}
	}

	ApplyMovement(dt, serverDirection, serverEffect);

	if (GetTransform().GetPosition().y <= -3) {
		GetTransform().SetPosition({ 0, 3, 0 });
	}
}