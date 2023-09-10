#include "NetworkState.h"

using namespace NCL;
using namespace CSC8503;

NetworkState::NetworkState(int id, std::vector<Vector3> pos, std::vector<Quaternion> ori, float t, int scoreA, int scoreB,
							std::vector<int> pnt, std::vector<Vector3> camFwd, Vector3 objectivePos) {
	snapshotID = id;
	isAcknowledged = false;
	positions = pos;
	orientations = ori;
	timer = t;
	aScore = scoreA;
	bScore = scoreB;
	paint = pnt;
	cameraFwd = camFwd;
	objectivePosition = objectivePos;
}

NetworkState::NetworkState(int numClients) {
	isAcknowledged = true;
	snapshotID = -1;
	timer = 60.0f;

	for (int i = 0; i < numClients; i++) {
		positions.emplace_back(Vector3());
		orientations.emplace_back(Quaternion());
		paint.emplace_back(-1);
		cameraFwd.emplace_back(Vector3());
	}

	aScore = 0;
	bScore = 0;
	objectivePosition = Vector3();
}

NetworkState::~NetworkState()	{
	positions.clear();
	orientations.clear();
	paint.clear();
	cameraFwd.clear();
}