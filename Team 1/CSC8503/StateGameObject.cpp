#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"


using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->MoveLeft(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->MoveRight(dt);
		}
	);

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{
			return this->counter > 6.0f;
		}
	));

	stateMachine->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->counter < 0.0f;
		}
	));

}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->AddForce({ 30, 0, 0 });
	counter += dt;

}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->AddForce({ -30, 0, 0 });
	counter -= dt;

}

StateGameObject2::StateGameObject2()
{
	counter = 0.0f;
	stateMachine2 = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->Move1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Move2(dt);
		}
	);
	stateMachine2->AddState(stateA);

	stateMachine2->AddState(stateB);

	stateMachine2->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{
			return this->counter > 6.0f;
		}
	));
	stateMachine2->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->counter < 0.0f;
		}
	));

}

StateGameObject2:: ~StateGameObject2()
{
	delete stateMachine2;
}

void StateGameObject2::Update(float dt)
{
	stateMachine2->Update(dt);
}

void StateGameObject2::Move1(float dt)
{
	GetPhysicsObject()->AddForce({ -30, 0, 0 });
	counter += dt;
}

void StateGameObject2::Move2(float dt)
{
	GetPhysicsObject()->AddForce({ 30, 0, 0 });
	counter -= dt;
}



TestStateThree::TestStateThree()
{
	counter = 0.0f;
	threeState = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);

	threeState->AddState(stateA);
	threeState->AddState(stateB);
	threeState->AddState(stateC);

	threeState->AddTransition(new StateTransition(stateA, stateB, stateC,
		[&]()-> bool
		{
			return this->counter > 30.0f;
		}
	));
	threeState->AddTransition(new StateTransition(stateB, stateC, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	threeState->AddTransition(new StateTransition(stateC, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 10.0f;
		}
	));

}
TestStateThree::~TestStateThree()
{
	delete threeState;
}

void TestStateThree::Update(float dt)
{
	threeState->Update(dt);
}

void TestStateThree::Go1(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, 3000, 0));
	counter += dt;
}

void TestStateThree::Go2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, -3000, 0));
	counter -= dt;
}

void TestStateThree::Go3(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, 3000, 0));
	counter += dt;
}

TestStateThree2::TestStateThree2()
{
	counter = 0.0f;
	threeState2 = new StateMachine();
	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);
	threeState2->AddState(stateA);
	threeState2->AddState(stateB);
	threeState2->AddState(stateC);
	threeState2->AddTransition(new StateTransition(stateA, stateB, stateC,
		[&]()-> bool
		{
			return this->counter > 30.0f;
		}
	));
	threeState2->AddTransition(new StateTransition(stateB, stateC, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	threeState2->AddTransition(new StateTransition(stateC, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 10.0F;
		}
	));
}
TestStateThree2::~TestStateThree2()
{
	delete threeState2;
}
void TestStateThree2::Update(float dt)
{
	threeState2->Update(dt);
}

void TestStateThree2::Go1(float dt)//add potion  
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, -3000, 0));
	counter += dt;
}

void TestStateThree2::Go2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, 3000, 0));
	counter -= dt;
}

void TestStateThree2::Go3(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	GetPhysicsObject()->AddTorque(Vector3(0, -3000, 0));
	counter += dt;
}

TestStateFour::TestStateFour()
{
	counter = 0.0f;
	fourState = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Go4(dt);
		}
	);

	fourState->AddState(stateA);
	fourState->AddState(stateB);
	fourState->AddState(stateC);
	fourState->AddState(stateD);

	fourState->AddTransition(new StateTransition(stateA, stateB, stateC,stateD,
		[&]()-> bool
		{
			return this->counter > 5.0f;
		}
	));
	fourState->AddTransition(new StateTransition(stateB, stateC,stateD, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	fourState->AddTransition(new StateTransition(stateC,stateD, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 5.0F;
		}
	));
	fourState->AddTransition(new StateTransition(stateD, stateA, stateB, stateC,
		[&]()->bool
		{
			return this->counter < 0.0F;
		}
	));
}
TestStateFour:: ~TestStateFour()
{
	delete fourState;
}

void TestStateFour::Update(float dt)
{
	fourState->Update(dt);
}

void TestStateFour::Go1(float dt) 
{
	GetPhysicsObject()->AddForce(Vector3(300, 0, 0));
	counter += dt;
}

void TestStateFour::Go2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 300));
	counter -= dt;
}

void TestStateFour::Go3(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(-300, 0, 0));
	counter += dt;
}
void TestStateFour::Go4(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, -300));
	counter -= dt;
}

TestStateFour2::TestStateFour2()
{
	counter = 2.0f;
	fourState2 = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Go4(dt);
		}
	);

	fourState2->AddState(stateA);
	fourState2->AddState(stateB);
	fourState2->AddState(stateC);
	fourState2->AddState(stateD);

	fourState2->AddTransition(new StateTransition(stateA, stateB, stateC, stateD,
		[&]()-> bool
		{
			return this->counter > 6.0f;
		}
	));
	fourState2->AddTransition(new StateTransition(stateB, stateC, stateD, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	fourState2->AddTransition(new StateTransition(stateC, stateD, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 6.0F;
		}
	));
	fourState2->AddTransition(new StateTransition(stateD, stateA, stateB, stateC,
		[&]()->bool
		{
			return this->counter < 0.0F;
		}
	));
}

TestStateFour2:: ~TestStateFour2()
{
	delete fourState2;
}

void TestStateFour2::Update(float dt)
{
	fourState2->Update(dt);
}

void TestStateFour2::Go1(float dt)  
{
	GetPhysicsObject()->AddForce(Vector3(-300, 0, 0));
	counter += dt;
}

void TestStateFour2::Go2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, -300));
	counter -= dt;
}

void TestStateFour2::Go3(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(300, 0, 0));
	counter += dt;
}
void TestStateFour2::Go4(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 300));
	counter -= dt;
}


SpecialState::SpecialState()
{
	counter = 0.0f;
	specialState = new StateMachine();

	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Go4(dt);
		}
	);

	specialState->AddState(stateA);
	specialState->AddState(stateB);
	specialState->AddState(stateC);
	specialState->AddState(stateD);

	specialState->AddTransition(new StateTransition(stateA, stateB, stateC, stateD,
		[&]()-> bool
		{
			return this->counter > 2.0f;
		}
	));
	specialState->AddTransition(new StateTransition(stateB, stateC, stateD, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	specialState->AddTransition(new StateTransition(stateC, stateD, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 2.0F;
		}
	));
	specialState->AddTransition(new StateTransition(stateD, stateA, stateB, stateC,
		[&]()->bool
		{
			return this->counter < 0.0F;
		}
	));
}
SpecialState:: ~SpecialState()
{
	delete specialState;
}

void SpecialState::Update(float dt)
{
	specialState->Update(dt);
}

void SpecialState::Go1(float dt)//add potion  
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	
	counter += dt;
}

void SpecialState::Go2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 300, 0));
	
	counter -= dt;
}

void SpecialState::Go3(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	
	counter += dt;
}
void SpecialState::Go4(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, -300, 0));
	
	counter -= dt;
}

DoorState::DoorState()
{
	counter = 0.0f;
	stateDoor = new StateMachine();
	State* stateA = new State([&](float dt)-> void
		{
			this->MoveUp(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->MoveDown(dt);
		}
	);
	stateDoor->AddState(stateA);
	stateDoor->AddState(stateB);

	stateDoor->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{
			return this->counter > 20.0f;
		}
	));
	stateDoor->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->counter < 0.0f;
		}
	));
}
DoorState::~DoorState()
{
	delete stateDoor;
}

void DoorState::Update(float dt)
{
	stateDoor->Update(dt);
}

void DoorState::MoveUp(float dt)
{
	GetPhysicsObject()->AddTorque(Vector3(1500,0,0));
	counter += dt;
}
void DoorState::MoveDown(float dt)
{
	GetPhysicsObject()->AddTorque(Vector3(-1500, 0, 0));
	counter -= dt;
}

DoorState2::DoorState2()
{
	counter = 0.0f;
	stateDoor = new StateMachine();
	State* stateA = new State([&](float dt)-> void
		{
			this->MoveUp(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->MoveDown(dt);
		}
	);
	stateDoor->AddState(stateA);
	stateDoor->AddState(stateB);

	stateDoor->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{
			return this->counter > 20.0f;
		}
	));
	stateDoor->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->counter < 0.0f;
		}
	));
}
DoorState2::~DoorState2()
{
	delete stateDoor;
}

void DoorState2::Update(float dt)
{
	stateDoor->Update(dt);
}

void DoorState2::MoveUp(float dt)
{
	GetPhysicsObject()->AddTorque(Vector3(-1500, 0, 0));
	counter += dt;
}
void DoorState2::MoveDown(float dt)
{
	GetPhysicsObject()->AddTorque(Vector3(1500, 0, 0));
	counter -= dt;
}

TheLift::TheLift()
{
	counter = 0.0f;
	theLift = new StateMachine();
	State* stateA = new State([&](float dt)-> void
		{
			this->MoveUp(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Stay(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->MoveDown(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Stay2(dt);
		}
	);

	theLift->AddState(stateA);
	theLift->AddState(stateB);
	theLift->AddState(stateC);
	theLift->AddState(stateD);

	theLift->AddTransition(new StateTransition(stateA, stateB, stateC, stateD,
		[&]()-> bool
		{
			return this->counter > 2.0f;
		}
	));
	theLift->AddTransition(new StateTransition(stateB, stateC, stateD, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	theLift->AddTransition(new StateTransition(stateC, stateD, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 6.0f;
		}
	));
	theLift->AddTransition(new StateTransition(stateD, stateA, stateB, stateC,
		[&]()->bool
		{
			return this->counter < 0.0F;
		}
	));

}
TheLift::~TheLift()
{
	delete theLift;
}
void TheLift::Update(float dt)
{
	theLift->Update(dt);
}
void TheLift::MoveUp(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 60000, 0));
	counter += dt;
}
void TheLift::Stay(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	counter -= dt;
}
void TheLift::MoveDown(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, -60000, 0));
	counter += dt;
}
void TheLift::Stay2(float dt)
{
	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	counter -= dt;
}
TheBeat::TheBeat()
{
	counter = 0.0f;
	theBeat = new StateMachine();
	State* stateA = new State([&](float dt)-> void
		{
			this->Go1(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->Go2(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Go3(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Go4(dt);
		}
	);

	theBeat->AddState(stateA);
	theBeat->AddState(stateB);
	theBeat->AddState(stateC);
	theBeat->AddState(stateD);

	theBeat->AddTransition(new StateTransition(stateA, stateB, stateC, stateD,
		[&]()-> bool
		{
			return this->counter > 3.0f;
		}
	));
	theBeat->AddTransition(new StateTransition(stateB, stateC, stateD, stateA,
		[&]()->bool
		{
			return this->counter < 0.0f;
		}
	));
	theBeat->AddTransition(new StateTransition(stateC, stateD, stateA, stateB,
		[&]()->bool
		{
			return this->counter > 3.0f;
		}
	));
	theBeat->AddTransition(new StateTransition(stateD, stateA, stateB, stateC,
		[&]()->bool
		{
			return this->counter < 0.0F;
		}
	));

}
TheBeat::~TheBeat()
{
	delete theBeat;
}
void TheBeat::Update(float dt)
{
	theBeat->Update(dt);
}
void TheBeat::Go1(float dt)
{
	
	GetPhysicsObject()->AddForce(Vector3(-30000,0 , 0));
	counter += dt;
}
void TheBeat::Go2(float dt)
{

	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	counter -= dt;
}
void TheBeat::Go3(float dt)
{
	
	GetPhysicsObject()->AddForce(Vector3(30000, 0, 0));
	counter += dt;
}
void TheBeat::Go4(float dt)
{

	GetPhysicsObject()->AddForce(Vector3(0, 0, 0));
	counter -= dt;
}

