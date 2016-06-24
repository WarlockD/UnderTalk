#pragma once
#include "Global.h"


enum class Step {
	Start,
	Normal,
	End
};

inline float getAngleOfTwoVectors(sf::Vector2f vec1, sf::Vector2f vec2)
{
	const float deg = 57.295779513082320876798154814105f;
	auto vectorFromVec1ToVec2 = vec2 - vec1;
	// the angle between two vectors
	return deg  * -vectorFromVec1ToVec2.getAngle();
}
inline void rotateNodeToPoint(sf::Transformable* node, sf::Vector2f point)
{
	float angleNodeToRotateTo = getAngleOfTwoVectors(node->getPosition(), point);
	float nodeCurrentAngle = getCurrentAngle(node);

	float diffAngle = getAngleDifference(angleNodeToRotateTo, nodeCurrentAngle);

	float rotation = nodeCurrentAngle + diffAngle;

	node->setRotation(rotation);
}

struct IPreStep {
	virtual void step_pre(float dt) = 0;
	virtual ~IPreStep() {}
};
struct IPostStep {
	virtual void step_post(float dt) = 0;
	virtual ~IPostStep() {}
};

class GObject : public sf::Transformable {
	sf::Vector2f _movment;
	float _direction;
	sf::Vector2f _speed;
public:
	inline virtual void step(float dt) { setPosition(getPosition() + _movment); }
	inline virtual void setDirection(float d) { _movment = CreateMovementVector(_direction = d, _speed); }
	inline virtual void setSpeed(float s) { _movment = CreateMovementVector(_direction, _speed= sf::Vector2f(s,s)); }
	inline virtual void setSpeed(sf::Vector2f s) { _movment = CreateMovementVector(_direction, _speed = s); }
	inline virtual sf::Vector2f getSpeed() const { return _speed; }
};