#pragma once
#include "DirectionComponent.h"



struct JumpComponent
{
	JumpComponent(Direction direction, float fromX, float fromY, float toX, float toY, float startVelocity, float totalTime, float deltaTimeMultiplikator) : direction(direction), fromX(fromX), fromY(fromY), toX(toX), toY(toY), startVelocity(startVelocity), totalTime(totalTime), deltaTimeMultiplikator(deltaTimeMultiplikator){}

	Direction direction;
	float fromX,fromY;
	float toX, toY;
	double totalTime, timePassed = 0;	//Je h�her totalTime um so h�her fliegt der Ball.
	float startVelocity;
	float degreeX, degreeY;
	bool isDegreeCalculated=false;
	float deltaTimeMultiplikator;		//Einflu� auf die Simulationsgeschwindigkeit, bei hohen werten ist der Sprung schnell
};