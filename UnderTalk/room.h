#pragma once
#include "Global.h"



class DrawableObject : public sf::Drawable {
	int _tag;
	int _depth;
public:
	DrawableObject() : _tag(0), _depth(0) {}
	virtual ~DrawableObject() {}
	int getTag() const { return _tag; }
	virtual void setTag(int tag) { _tag = tag; }
	int getDepth() const { return _depth; }
	virtual void setDepth(int depth) { _depth = depth; }
	virtual void step(float dt) = 0;
	bool operator==(const DrawableObject& d) const { return &d == this || _tag == d._tag; } // pointer equal or tag equal
	bool operator!=(const DrawableObject& d) const { return !(d == *this); }
	bool operator<(const DrawableObject& d) const { return _depth < d._depth; } // we sort by depth
};
namespace std {
	template<> struct less<DrawableObject> { bool operator() (const DrawableObject& lhs, const DrawableObject& rhs) const { return lhs.getDepth() < rhs.getDepth(); } };
	template<> struct less<DrawableObject*> { bool operator() (const DrawableObject* lhs, const DrawableObject* rhs) const { return lhs->getDepth() < rhs->getDepth(); } };
}

class Room;

class RoomObject : public DrawableObject, public sf::Transformable {
private: // simple movment stuff and physics stuff
	sf::Vector2f _movmentVector;
	sf::Vector2f _gravityVector;
	sf::Vector2f _velocityVector;
	float _gravity;
	float _gravityDirection;
	float _direction;
	float _speed;
public:
	const sf::Vector2f getNextPosition(float dt) const {
		sf::Vector2f pos = getPosition();
		pos += _movmentVector  * dt; // add the movment vector first
		pos += _velocityVector  * dt; // add the gravity acceration
		return pos;
	}
	void setDirection(float d) { _movmentVector = CreateMovementVector(_direction = d,_speed);  }
	void setSpeed(float s) { _movmentVector = CreateMovementVector(_direction, _speed=s); }
	void setGravityDirection(float d) { _gravityVector = CreateMovementVector(_direction = d, _gravity);  _velocityVector = sf::Vector2f(); }
	void setGravity(float s) { _gravityVector = CreateMovementVector(_direction, _gravity=s); _velocityVector = sf::Vector2f();}
	float getGravity() const { return _gravity; }
	float getSpeed() const { return _speed; }

	const sf::Vector2f& getMovement() const { return _movmentVector; }
	sf::Vector2f& getMovement() { return _movmentVector; }
	const sf::Vector2f& getVelocity() const { return _velocityVector; }
	sf::Vector2f& getVelocity() { return _velocityVector; }
private:
	friend class Room;
	Room* _room;
	RoomObject(Room* room);
	bool _visiable;
protected:
	sf::VertexArray _vertices;
	const sf::Texture* _texture; 
public:
	Room& getRoom() const  { return *_room; }
	virtual ~RoomObject();
	bool isVisiable() const { return _visiable; }
	void setVisiable(bool v) { _visiable=v;  }
	virtual const sf::Texture* getTexture() const = 0;
	virtual void step(float dt) {
		sf::Vector2f pos = getNextPosition(dt);
		if (_gravity != 0.0f) {	//update
			_velocityVector += _gravityVector * dt;// velocity += timestep * acceleration;	
		}
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (_visiable && _texture != nullptr && _vertices.getVertexCount() > 0) {
			states.transform *= getTransform();
			states.texture = _texture;
			target.draw(_vertices, states);
		}
	}
};
// simple tilemap from examples
class TileMap : public sf::Drawable, public sf::Transformable
{
	sf::Texture _texture;
	sf::VertexArray _vertices;
	const Undertale::Room* _room;
public:
	TileMap(Undertale::UndertaleFile& res, const Undertale::Room* room);
	void loadRoom(Undertale::UndertaleFile& res, const Undertale::Room* room);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = &_texture;
		target.draw(_vertices, states);
	}
};

class Room {
	friend class RoomObject;
	struct RoomObjectDepthSort {
		bool operator() (const RoomObject& lhs, const RoomObject& rhs) const {
			return lhs.getDepth() < rhs.getDepth();
		}
		bool operator() (const RoomObject* lhs, const RoomObject* rhs) const {
			return lhs->getDepth() < rhs->getDepth();
		}
	};
	typedef std::unordered_set<RoomObject*> t_objectList;
	typedef t_objectList::iterator t_objectList_it;
	typedef std::set<DrawableObject*> t_sortedObjectList;
	typedef t_sortedObjectList::iterator t_sortedObjectList_it;

	t_objectList _objects; // this is used for quick lookup
	t_sortedObjectList _sortedObjects; // this is used for depth order
	void resortObjects();
	void addChild(RoomObject* object);
	void removeChild(RoomObject* object);
	void removeChild(int tag);
	sf::View _vew;
	bool _objectsChanged;
	

public:
	virtual void step(float dt) {
		for (auto o : _objects) o->step(dt);
		if (_objectsChanged) { resortObjects(); _objectsChanged = false; }
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  {

	}
};