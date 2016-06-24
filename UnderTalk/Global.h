#pragma once
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "json.hpp"
#include <fstream>
#include <map>

// interfaces used when you need to run before or after the main step
inline float lengthdir_x(float len, float dir) {
	const double pi = 3.1415926535897;
	return std::cosf(dir* 0.01745329252f) * len;
}
inline float lengthdir_y(float len, float dir) {
	const float rad = 0.01745329252f;
	return -std::sinf(dir* rad) * len;
}
inline sf::Vector2f CreateMovementVector(float dir, float speed) {
	const float rad = 0.01745329252f;
	float x = std::cosf(dir* rad) * speed;
	float y = std::sinf(dir* rad) * speed;
	return sf::Vector2f(x, y);
}
inline sf::Vector2f CreateMovementVector(float dir, sf::Vector2f speed) {
	const float rad = 0.01745329252f;
	float x = std::cosf(dir* rad) * speed.x;
	float y = std::sinf(dir* rad) * speed.y;
	return sf::Vector2f(x, y);
}
// http://discuss.cocos2d-x.org/t/rotate-sprite-towards-a-point/16850/2
// useful functions
inline float getAngleDifference(float angle1, float angle2)
{
	float diffAngle = (angle1 - angle2);

	if (diffAngle >= 180.f)
	{
		diffAngle -= 360.f;
	}
	else if (diffAngle < -180.f)
	{
		diffAngle += 360.f;
	}

	// how much angle the node needs to rotate
	return diffAngle;
}
inline float getCurrentAngle(sf::Transformable* node)
{
	float rotAng = node->getRotation();

	if (rotAng >= 180.f)
	{
		rotAng -= 360.f;
	}
	else if (rotAng < -180.f)
	{
		rotAng += 360.f;
	}

	// negative angle means node is facing to its left
	// positive angle means node is facing to its right
	return rotAng;
}