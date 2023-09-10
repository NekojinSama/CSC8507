#pragma once

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class NetworkState	{
		public:
			NetworkState(int id, std::vector<Vector3> pos, std::vector<Quaternion> ori, float t, int scoreA, int scoreB,
							std::vector<int> pnt, std::vector<Vector3> camFwd, Vector3 objectivePos);
			NetworkState(int numClients);
			virtual ~NetworkState();

			int snapshotID;
			std::vector<Vector3> positions;
			std::vector<Quaternion> orientations;
			std::vector<int> paint;
			std::vector<Vector3> cameraFwd;
			Vector3 objectivePosition;
			bool isAcknowledged;

			float timer;
			int aScore, bScore;
		};
	}
}

