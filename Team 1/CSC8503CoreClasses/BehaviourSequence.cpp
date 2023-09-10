//#include "BehaviourSequence.h"
//#include <iostream>
//BehaviourState BehaviourSequence::Execute(float dt) {
//	for (auto& i : childNodes) {
//		BehaviourState nodeState = i->Execute(dt);
//		switch (nodeState) {
//			case Success: continue;
//			case Failure: 
//			case Ongoing:
//			{
//				currentState = nodeState;
//				return nodeState;
//			}
//		}
//	}
//	return Success;
//}