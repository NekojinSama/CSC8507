#pragma once
#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject();
            ~StateGameObject();
            virtual void Update(float dt);
        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);

            StateMachine* stateMachine;
            float counter;
        };

        class StateGameObject2 : public GameObject
        {
        public:
            StateGameObject2();
            ~StateGameObject2();
            virtual void Update(float dt);

        protected:
            void Move1(float dt);
            void Move2(float dt);
            StateMachine* stateMachine2;
            float counter;
        };

        class TestStateThree : public GameObject
        {
        public:
            TestStateThree();
            ~TestStateThree();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);

            StateMachine* threeState;
            float counter;
        };

        class TestStateThree2 : public GameObject
        {
        public:
            TestStateThree2();
            ~TestStateThree2();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);

            StateMachine* threeState2;
            float counter;

        };

        class TestStateFour : public GameObject
        {
        public:
            TestStateFour();
            ~TestStateFour();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);
            void Go4(float dt);
            StateMachine* fourState;
            float counter;
        };

        class TestStateFour2 : public GameObject
        {
        public:
            TestStateFour2();
            ~TestStateFour2();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);
            void Go4(float dt);
            StateMachine* fourState2;
            float counter;
        };

        class SpecialState : public GameObject
        {
        public:
            SpecialState();
            ~SpecialState();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);
            void Go4(float dt);
            StateMachine* specialState;
            float counter;

        };

        class DoorState : public GameObject {
        public:
            DoorState();
            ~DoorState();
            virtual void Update(float dt);
        protected:
            void MoveUp(float dt);
            void MoveDown(float dt);

            StateMachine* stateDoor;
            float counter;
        };

        class DoorState2 : public GameObject {
        public:
            DoorState2();
            ~DoorState2();
            virtual void Update(float dt);
        protected:
            void MoveUp(float dt);
            void MoveDown(float dt);

            StateMachine* stateDoor;
            float counter;
        };
        class TheLift :public GameObject
        {
        public:
            TheLift();
            ~TheLift();
            virtual void Update(float dt);
        protected:
            void MoveUp(float dt);
            void Stay(float dt);
            void MoveDown(float dt);
            void Stay2(float dt);

            StateMachine* theLift;
            float counter;
        };

        class TheBeat :public GameObject
        {
        public:
            TheBeat();
            ~TheBeat();
            virtual void Update(float dt);
        protected:
            void Go1(float dt);
            void Go2(float dt);
            void Go3(float dt);
            void Go4(float dt);

            StateMachine* theBeat;
            float counter;
        };
    }
}
