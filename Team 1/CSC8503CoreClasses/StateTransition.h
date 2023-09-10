#pragma once

namespace NCL {
	namespace CSC8503 {
		class State;
		typedef std::function<bool()> StateTransitionFunction;
		class StateTransition	{
		public:
			StateTransition(State* source, State* dest, StateTransitionFunction f) {
				sourceState		 = source;
				destinationState = dest;
				function		 = f;
			}

			bool CanTransition() const {
				return function();
			}

			State* GetDestinationState()  const {
				return destinationState;
			}

			State* GetSourceState() const {
				return sourceState;
			}

			StateTransition(State* source, State* midgo, State* dest, StateTransitionFunction f)
			{
				sourceState = source;
				midwalk = midgo;
				destinationState = dest;
				function = f;
			}
			State* GetMidSituation() const
			{
				return midwalk;
			}

			StateTransition(State* source, State* midgo, State* thrgo, State* dest, StateTransitionFunction f)
			{
				sourceState = source;
				thirdwalk = thrgo;
				midwalk = midgo;
				destinationState = dest;
				function = f;
			}
			State* GetThirdSituation() const
			{
				return thirdwalk;
			}


		protected:
			State * sourceState;
			State * destinationState;
			State* midwalk;
			State* thirdwalk;
			StateTransitionFunction function;
		};
	}
}

