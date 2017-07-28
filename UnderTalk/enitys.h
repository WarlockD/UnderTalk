#pragma once
#include "Global.h"
#include "Global.h"

#include "node.h"
#include "gsprites.h"


// simple tilemap from examples
class TileMap : public sf::Drawable
{
	sf::RenderTexture _tiles;
	sf::Vertex _vertices[4]; // We draw to a render texture and this is the size of it
	gm::Room _room;
	bool _hasTiles;
public:
	TileMap() : _hasTiles(false) {}
	void loadRoom(const gm::Room& room);
	void unloadRoom();
	//TileMap(size_t index);
	//void loadRoom(size_t index);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


class SpriteComponent : public GSprite, public sf::Drawable {
public:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (valid()) {
			states.texture = getTexture().get();
			target.draw(getVertices(), getVerticesCount(), getVerticesType(), states);
		}
	}
};
using PositionComponent = sf::Transformable;

using ObjectComponent = Undertale::Room::Object;

class StaticComponent {
	sf::FloatRect _rect;
public:
	StaticComponent() : _rect() {}
	StaticComponent(const sf::FloatRect& box) : _rect(box) {}
	const sf::FloatRect getRect() const { return _rect; }
	void setRect(const sf::FloatRect& rect) { _rect = rect; }
};

class SimpleTimer {
	float _time;
	float _alarm;
public:
	SimpleTimer(float alarm) : _time(0.0f), _alarm(alarm) {}
	void setAlarm(float alarm) { _alarm = alarm; }
	float getAlarm() const { return _alarm; }
	void reset() { _time = 0.0f; }
	bool addTime(float dt) {
		_time += dt;
		if (_time >= _alarm) {
			_time = 0;
			return true;
		}
		return false;
	}
};

class AnimationComponent {
	SimpleTimer _alarm;
	bool _running;
	bool _forward;
public:
	AnimationComponent(float speed) :  _alarm(speed), _running(false) , _forward(true) {}
	void setSpeed(float fps) { _alarm.setAlarm(fps); }
	void start() { _running = true; }
	void stop() { _running = false; _alarm.reset(); }
	void pause() { _running = false; }
	float getSpeed() const { return _running ? _alarm.getAlarm() : false; }
	bool checkFrame(float dt) { return _alarm.addTime(dt); }
	bool checkFrame(float dt, GSprite& sprite) {
		if (checkFrame(dt)) {
			size_t next = sprite.getImageIndex();
			if (_forward) {
				++next;
				if (next >= sprite.getImageCount()) next = 0;
			}
			else {
				if (next == 0) next = sprite.getImageCount() - 1;
				else --next;
			}
			sprite.setImageIndex(next);
			return true;
		}
		return false;
	}
};
using CompList = entityplus::component_list<PositionComponent,VelocityComponent, GravityComponent, SpriteComponent, StaticComponent, DepthComponent, AnimationComponent, ObjectComponent>;
struct PlayerTag {}; // tag


using TagList = entityplus::tag_list<PlayerTag>;
using ManagerType = entityplus::entity_manager<CompList, TagList>;
using entity_t = typename ManagerType::entity_t;



#if 0
Position(x, y)
Velocity(x, y)
Physics(body)
Sprite(images, animations)
Health(value)
Character(name, level)
Player(empty)
#endif