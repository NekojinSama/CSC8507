//https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/

#pragma once
#include <Windows.h>
#include <Xinput.h>
#include "Controller.h"
#pragma comment(lib,"Xinput9_1_0.lib")

class Gamepad : public Controller {
public:
	Gamepad(int controllerNumber);
	Gamepad();

	void UpdateController() override;

	bool GetInputDown(ControllerInputs input)override;
	bool GetInputHeld(ControllerInputs input)override;
	bool GetInputPressed(ControllerInputs input)override;

	bool GetTriggerDown(bool trigger)override;

	float GetYaw(float y)override;
	float GetPitch(float p)override;
	float GetLeftAnalogStickX()override;
	float GetLeftAnalogStickY()override;

	bool ControllerState()override;
protected:
	XINPUT_STATE state;

	bool holdInputs[c_LAST];
	bool inputsStates[c_LAST];
	int gamepadKeys[10];

	int controllerPort;

	float deadzoneX = 0.15f;
	float deadzoneY = 0.15f;
};