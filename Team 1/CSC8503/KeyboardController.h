#pragma once
#include <Windows.h>
#include <Xinput.h>
#include "Controller.h"
#pragma comment(lib,"Xinput9_1_0.lib")

class KeyboardController : public Controller {
public:
	KeyboardController(int useKeyboard);
	KeyboardController();

	void UpdateController() override;

	bool GetInputDown(ControllerInputs input)override;
	bool GetInputHeld(ControllerInputs input)override;
	bool GetInputPressed(ControllerInputs input)override;

	bool GetTriggerDown(bool trigger)override;

	float GetYaw(float y)override;
	float GetPitch(float pint )override;
	float GetLeftAnalogStickX()override;
	float GetLeftAnalogStickY()override;

	bool ControllerState()override;
protected:
	int state;


	int Keys[c_LAST];
};
