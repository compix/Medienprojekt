#pragma once
#include "DirectionComponent.h"



struct JumpComponent
{
	JumpComponent(Direction direction, int fromX, int fromY, int toX, int toY, float startVelocity, float totalTime, float deltaTimeMultiplikator) 
		: direction(direction), fromX(fromX), fromY(fromY), toX(toX), toY(toY), startVelocity(startVelocity), 
		totalTime(totalTime), deltaTimeMultiplikator(deltaTimeMultiplikator){}

	Direction direction;
	int fromX, fromY;
	int toX, toY;
	float totalTime, timePassed = 0;	//Je h�her totalTime um so h�her fliegt der Ball.
	float startVelocity;
	float degreeX, degreeY;
	bool isDegreeCalculated=false;
	float deltaTimeMultiplikator;		//Einflu� auf die Simulationsgeschwindigkeit, bei hohen werten ist der Sprung schnell
	bool targetIsBlocked, targetWasBlocked, wasBlocked = false; //Die ersten zwei bezihen sich auf das Ziel, wasBlocked bezieht sich auf die Zelle von dem er kommt.
};