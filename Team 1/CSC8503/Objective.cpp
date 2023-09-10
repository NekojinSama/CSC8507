#include "Objective.h"
#include "Player.h"

Objective::Objective(int* _aScore, int* _bScore) : GameObject() {
	name = "Objective";
	collected = false;
	aScore = _aScore;
	bScore = _bScore;

	Vector3 temp[9] = { Vector3(0,2,0) , Vector3(111,18,252),Vector3(-111,18,252), 
		Vector3(-111,18,-252), Vector3(111,18,-252), Vector3(75,18, 72), 
		Vector3(-75,18,-72), Vector3(75,2, -72), Vector3(75,2,-72) };

	std::copy(temp, temp + 9, locations);
}

Objective::~Objective() {
	
}

void Objective::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "Player" && !collected) {
		collected = true;

		switch (((Player*)otherObject)->GetTeam()) {
		case 0:
			(*aScore)++;
			break;
		case 1:
			(*bScore)++;
			break;
		}
	}
}

void Objective::OnCollisionEnd(GameObject* otherObject) {
	if (otherObject->GetName() == "Player" && collected) {
		int x = ((rand() % 4));
		last = last == x ? last = 4 : last = x;;
		GetTransform().SetPosition(locations[last]);
		flag->GetTransform().SetPosition(locations[last] + Vector3(-10, 45, 0));
		collected = false;
	}
}