#include "Controller.h"

void Controller::UpdateController() {}

bool Controller::GetInputDown(ControllerInputs input) { return false; }
bool Controller::GetInputHeld(ControllerInputs input) { return false; }
bool Controller::GetInputPressed(ControllerInputs input) { return false; }

bool Controller::GetTriggerDown(bool trigger) { return false; }

float Controller::GetYaw(float y) { return 0.0f; }
float Controller::GetPitch(float p) { return 0.0f; }
float Controller::GetLeftAnalogStickX() { return 0.0f; }
float Controller::GetLeftAnalogStickY() { return 0.0f; }

bool Controller::ControllerState() { return false; }