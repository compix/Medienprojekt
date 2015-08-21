#pragma once
#include "DirectionComponent.h"



struct JumpComponent
{
	JumpComponent(Direction direction, int fromX, int fromY, int toX, int toY, float startVelocity, float totalTime, float deltaTimeMultiplikator) : direction(direction), fromX(fromX), fromY(fromY), toX(toX), toY(toY), startVelocity(startVelocity), totalTime(totalTime), deltaTimeMultiplikator(deltaTimeMultiplikator){}

	Direction direction;
	int fromX, fromY;
	int toX, toY;
	double totalTime, timePassed = 0;	//Je höher totalTime um so höher fliegt der Ball.
	float startVelocity;
	float degreeX, degreeY;
	bool isDegreeCalculated=false;
	float deltaTimeMultiplikator;		//Einfluß auf die Simulationsgeschwindigkeit, bei hohen werten ist der Sprung schnell
	bool targetIsBlocked, wasBlocked = false;
};