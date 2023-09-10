#pragma once

#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class Objective : public GameObject {
public:
	Objective(int* _aScore, int* _bScore);

	~Objective();

	virtual void OnCollisionBegin(GameObject* otherObject);
	virtual void OnCollisionEnd(GameObject* otherObject);

	
	GameObject* flag;
protected:
	void Respawn();
	bool collected;
	int* aScore;
	int* bScore;
	int last;

	Vector3 locations[9];
};