#pragma once
#include "DirectionComponent.h"



struct JumpComponent
{
	JumpComponent()
		: direction(Direction::DOWN), fromX(0), fromY(0), toX(0), toY(0), startVelocity(0),
		totalTime(0), deltaTimeMultiplikator(0){}
	JumpComponent(Direction direction, int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, float startVelocity, float totalTime, float deltaTimeMultiplikator)
		: direction(direction), fromX(fromX), fromY(fromY), toX(toX), toY(toY), startVelocity(startVelocity), 
		totalTime(totalTime), deltaTimeMultiplikator(deltaTimeMultiplikator){}

	Direction direction;
	int8_t fromX, fromY;
	int8_t toX, toY;
    float startVelocity;
    float totalTime, timePassed = 0;	//Je höher totalTime um so höher fliegt der Ball.
	float degreeX, degreeY;
	bool isDegreeCalculated=false;
	float deltaTimeMultiplikator;		//Einfluß auf die Simulationsgeschwindigkeit, bei hohen werten ist der Sprung schnell
	bool targetIsBlocked, targetWasBlocked, wasBlocked = false; //Die ersten zwei bezihen sich auf das Ziel, wasBlocked bezieht sich auf die Zelle von dem er kommt.
};
