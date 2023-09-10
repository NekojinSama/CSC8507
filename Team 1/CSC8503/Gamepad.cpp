#include "Gamepad.h"
#include "ConfigOptions.h"

Gamepad::Gamepad() {

}

Gamepad::Gamepad(int controllerNumber) {
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	controllerPort = controllerNumber;
	memset(holdInputs, 0, c_LAST * sizeof(bool));
	memset(inputsStates, 0, c_LAST * sizeof(bool));

	int temp[c_LAST] = { XINPUT_GAMEPAD_DPAD_UP,
		XINPUT_GAMEPAD_DPAD_RIGHT,
		XINPUT_GAMEPAD_DPAD_DOWN,
		XINPUT_GAMEPAD_DPAD_LEFT,
		XINPUT_GAMEPAD_Y,
		XINPUT_GAMEPAD_B,
		XINPUT_GAMEPAD_A,
		XINPUT_GAMEPAD_X,
		XINPUT_GAMEPAD_RIGHT_SHOULDER,
		XINPUT_GAMEPAD_LEFT_SHOULDER };
	std::copy(temp, temp + c_LAST, gamepadKeys);
}

void Gamepad::UpdateController() {
	memcpy(holdInputs, inputsStates, c_LAST);
	memset(inputsStates, 0, c_LAST * sizeof(bool));
}

bool Gamepad::GetInputDown(ControllerInputs input) {
	ControllerState();
	holdInputs[input] = (state.Gamepad.wButtons & gamepadKeys[input]) != 0;
	return holdInputs[input];
}

bool Gamepad::GetInputHeld(ControllerInputs input) {
	ControllerState();
	if (holdInputs[input] && GetInputDown(input)) {
		return true;
	}
	return false;
}

bool Gamepad::GetInputPressed(ControllerInputs input) {
	ControllerState();
	return (!GetInputHeld(input) && GetInputDown(input));
}

bool Gamepad::GetTriggerDown(bool trigger) {
	if (trigger)
		return state.Gamepad.bRightTrigger > 0 ? true : false;
	else
		return state.Gamepad.bLeftTrigger > 0 ? true : false;
}

float Gamepad::GetYaw(float y) {
	float analogX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
	return y - (abs(analogX) > deadzoneX ? analogX : 0) * NCL::CSC8503::ConfigOptions::sensitivity;
}

float Gamepad::GetPitch(float p) {
	float analogY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);
	return p + (abs(analogY) > deadzoneY ? analogY : 0) * NCL::CSC8503::ConfigOptions::sensitivity;
}

float Gamepad::GetLeftAnalogStickX() {
	float analogX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
	return abs(analogX) > deadzoneX ? analogX : 0;
}

float Gamepad::GetLeftAnalogStickY() {
	float analogY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);
	return abs(analogY) > deadzoneY ? analogY : 0;
}

bool Gamepad::ControllerState() {
	return XInputGetState(controllerPort, &state) == ERROR_SUCCESS;
}