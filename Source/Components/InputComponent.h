#pragma once
#include <cinttypes>

struct InputComponent
{
	float moveX = 0;
	float moveY = 0;
	bool bombButtonPressed = false;
	bool skillButtonPressed = false;
	bool disabled = false;
};