#pragma once


enum ControllerInputs
{
	c_UP,
	c_RIGHT,
	c_DOWN,
	c_LEFT,
	c_Y,
	c_B,
	c_A,
	c_X,
	c_RIGHTBUMPER,
	c_LEFTBUMPER,
	c_LAST
};

class Controller
{

public:
	virtual void UpdateController();

	virtual bool GetInputDown(ControllerInputs input);
	virtual bool GetInputHeld(ControllerInputs input);
	virtual bool GetInputPressed(ControllerInputs input);

	virtual bool GetTriggerDown(bool trigger);

	virtual float GetYaw(float y);
	virtual float GetPitch(float p);
	virtual float GetLeftAnalogStickX();
	virtual float GetLeftAnalogStickY();

	virtual bool ControllerState();

protected:

};