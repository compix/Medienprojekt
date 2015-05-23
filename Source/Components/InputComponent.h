#pragma once

struct InputComponent
{
	InputComponent() : bombButtonPressed(false), skillButtonPressed(false), moveX(0.f), moveY(0.f) {}

	int playerIndex;
	float moveX;
	float moveY;
	bool bombButtonPressed;
	bool skillButtonPressed;
};