#pragma once
#include "CollisionVolume.h"
#include "Vector3.h"

namespace NCL {
	using namespace NCL::Maths;
	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

		Vector3 UpdateDimensions(Vector3 d) {
			halfSizes *= d;
		}
	protected:
		Maths::Vector3 halfSizes;
	};
}

