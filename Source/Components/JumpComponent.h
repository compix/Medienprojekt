#pragma once
#include "DirectionComponent.h"



struct JumpComponent
{
	JumpComponent(Direction direction, float fromX, float fromY, float toX, float toY, float startVelocity, float totalTime, float deltaTimeMultiplikator) : direction(direction), fromX(fromX), fromY(fromY), toX(toX), toY(toY), startVelocity(startVelocity), totalTime(totalTime), deltaTimeMultiplikator(deltaTimeMultiplikator){}

	Direction direction;
	float fromX,fromY;
	float toX, toY;
	double totalTime, timePassed = 0;	//Je höher totalTime um so höher fliegt der Ball.
	float startVelocity;
	float degreeX, degreeY;
	bool isDegreeCalculated=false;
	float deltaTimeMultiplikator;		//Einfluß auf die Simulationsgeschwindigkeit, bei hohen werten ist der Sprung schnell
	bool targetIsBlocked = false;
};