#pragma once
#include "Global.h"


// a hacked implmentation of the node in cocos2d-x
class Room;
// super simple event system


class VelocityComponent {
	sf::Vector2f _movmentVector;
	float _speed;
	float _direction;
public:
	VelocityComponent() : _movmentVector(), _speed(0.0f) {}
	void setDirection(float d) { _movmentVector = CreateMovementVector(_direction = d, _speed); }
	void setSpeed(float s) { _movmentVector = CreateMovementVector(_direction, _speed = s); }
	void resetMovment() { _speed = 0.0f; _movmentVector = sf::Vector2f(); }
	float getSpeed() const { return _speed; }
	float getDirection() const { return _direction; }
	const sf::Vector2f& getMovementVector() const { return _movmentVector; }
	inline  sf::Vector2f getNextVector(sf::Vector2f position, float delta_time) const {
		position += _movmentVector  * delta_time; // add the movment vector first
		return position;
	}
};

class GravityComponent {
	sf::Vector2f _gravityVelocityVector;
	sf::Vector2f _gravityVector;
	float _gravity;
	float _gravityDirection;
public:
	GravityComponent() : _gravityVelocityVector(), _gravityVector(), _gravity(0.0f), _gravityDirection(0.0f) {}
	void setGravityDirection(float d) { _gravityVector = CreateMovementVector(_gravityDirection = d, _gravity);  _gravityVelocityVector = sf::Vector2f(); }
	void setGravity(float s) { _gravityVector = CreateMovementVector(_gravityDirection, _gravity = s); _gravityVelocityVector = sf::Vector2f(); }
	float getGravity() const { return _gravity; }
	float getGravityDirection() const { return _gravityDirection; }
	inline sf::Vector2f getNextVector(sf::Vector2f position, float delta_time)  {
		if (_gravity != 0.0f) {
			position += _gravityVelocityVector  * delta_time; // add the gravity acceration
			_gravityVelocityVector += _gravityVector * delta_time;// velocity += timestep * acceleration;	
		}	
		return position;
	}
};

class DepthComponent {
	float _depth;
public:
	DepthComponent(float depth = 0.0f) : _depth(depth) {}
	float getDepth() const { return _depth; }
	void setDepth(float depth) { _depth = depth; }
};

template<typename T>
class ConstantMovement : public VelocityComponent {
public:
	using base = T;
	inline  sf::Vector2f getNextVector(float delta_time) const {
		T* tf = static_cast<T*>(this);
		return getNextVector(tf->getPosition(), delta_time)
	}
	inline void movePosition(float delta_time) {
		T* tf = static_cast<T*>(this);
		tf->setPosition(getNextVector(tf->getPosition(), delta_time));
	}
};
template<typename T>
class GravityMovement : public GravityComponent {
public:
	using base = T;
	inline  sf::Vector2f getNextVector(float delta_time) const {
		T* tf = static_cast<T*>(this);
		return getNextVector(tf->getPosition(), delta_time)
	}
	inline void movePosition(float delta_time) {
		T* tf = static_cast<T*>(this);
		tf->setPosition(getNextVector(tf->getPosition(), delta_time));
	}
};

template<typename EVENT>
class NodeEvent {
public:
	using type = NodeEvent<EVENT>;
	using value_type = EVENT;
	virtual bool event(const EVENT& e) = 0;
	static std::unordered_set<type*>& instance() {
		static std::unordered_set<type*> _instance;
		return _instance;
	}
	NodeEvent() { instance().emplace(this); }
	virtual ~NodeEvent() {  instance().erase(this); } 
	static bool brodcast(const EVENT& e) {
		for (auto& o : instance()) {
			if (o->event(e)) return true;
		}
		return false;
	}
};
