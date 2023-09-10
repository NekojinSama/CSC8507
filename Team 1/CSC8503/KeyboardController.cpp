#include "KeyboardController.h"
#include "Window.h"
#include "ConfigOptions.h"

using namespace NCL;

KeyboardController::KeyboardController(int useKeyboard): state(useKeyboard) {

	int temp[c_LAST] = { (int)KeyboardKeys::W,
		(int)KeyboardKeys::D,
		(int)KeyboardKeys::S,
		(int)KeyboardKeys::A,
		(int)KeyboardKeys::Q,
		(int)KeyboardKeys::C,
		(int)KeyboardKeys::SPACE,
		(int)KeyboardKeys::E,
		(int)KeyboardKeys::NUM2,
		(int)KeyboardKeys::NUM1 };
	std::copy(temp, temp + c_LAST, Keys);
}

KeyboardController::KeyboardController(): state(false){}

void KeyboardController::UpdateController() {}

bool KeyboardController::GetInputDown(ControllerInputs input) {
	ControllerState();
	return (Window::GetKeyboard()->KeyDown((KeyboardKeys)Keys[input]));
}
bool KeyboardController::GetInputHeld(ControllerInputs input) {
	return (Window::GetKeyboard()->KeyHeld((KeyboardKeys)Keys[input]));
}
bool KeyboardController::GetInputPressed(ControllerInputs input) { 
	return (Window::GetKeyboard()->KeyPressed((KeyboardKeys)Keys[input])); 
}

bool KeyboardController::GetTriggerDown(bool trigger) { 
	if (trigger) 
		return Window::GetMouse()->ButtonDown(MouseButtons::LEFT);
	else 
		return Window::GetMouse()->ButtonDown(MouseButtons::RIGHT);
}

float KeyboardController::GetYaw(float y) { 
	return y - (Window::GetMouse()->GetRelativePosition().x) * NCL::CSC8503::ConfigOptions::sensitivity;
}
float KeyboardController::GetPitch(float p) { 
	return p - (Window::GetMouse()->GetRelativePosition().y) * NCL::CSC8503::ConfigOptions::sensitivity;
}
float KeyboardController::GetLeftAnalogStickX() { return false; }
float KeyboardController::GetLeftAnalogStickY() { return false; }

bool KeyboardController::ControllerState() { return state; }
