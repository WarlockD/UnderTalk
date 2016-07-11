#pragma once
#include "Global.h"
#include "gsprites.h"

class DrawableObject;
namespace std {
	template<>
	struct less<DrawableObject> {
		bool operator() (const DrawableObject& lhs, const DrawableObject& rhs) const;
		bool operator() (const DrawableObject* lhs, const DrawableObject* rhs) const;
	};
}

class DrawableObject : public sf::Drawable {
	DrawableObject* _root;
	DrawableObject* _parent;
	int _tag;
	int _depth;
	bool _visiable;
	// why not a list of std::unique_ptrs?  ~DrawableObject removes it from the list
	// and we might want to use a stack object for someing quick.  Otherwise we will have
	// a vector heap for containing the objects

	typedef std::unordered_set<DrawableObject*> t_objectList;
	typedef std::list<DrawableObject*> t_sortedObjectList;
	typedef t_objectList::iterator t_objectList_it;
	typedef t_sortedObjectList::iterator t_sortedObjectList_it;

	// wanted this to be a list of unique ptrs
	// but searching for them was slow and since I am managing them myself
	// we will try it this way
	bool _onheap;

	t_objectList _objects; // this is used for quick lookup
	t_sortedObjectList _sortedObjects; // this is used for depth order

	
	bool _objectsChanged;
	DrawableObject* getRoot() { if (getParent() == nullptr) return this; else return getRoot(); }
	void addSelfandRecursiveOthers(t_sortedObjectList& list);
protected:
	// override this to draw this object
	virtual void self_draw(sf::RenderTarget& target, sf::RenderStates states) const {}
	virtual void self_step(float dt) {}
public:
	DrawableObject() : _tag(0), _depth(0), _visiable(false), _objectsChanged(false), _parent(nullptr), _onheap(false) {}
	DrawableObject(const DrawableObject& copy) = delete;
	DrawableObject& operator=(const DrawableObject& copy) = delete; // not copyable
	DrawableObject(DrawableObject&& copy) = default;
	DrawableObject& operator=(DrawableObject&& copy) = default; // not copyable.  It can be moved though

	// this pointer is managed on root
	template<class DO>DO* create(int tag=0) {// create object linked to this parrent
		DrawableObject* root = getRoot();
		DO* obj = new DO();
		obj->_onheap = true;
		obj->addParent(this);
		return obj;
	}
	virtual ~DrawableObject();
	bool isVisible() const { return _visiable; }
	void setVisible(bool value) { _visiable = value; _objectsChanged = true; }
	DrawableObject* getParent() { return _parent; }
	void setParent(DrawableObject* parent);


	const DrawableObject* getParent() const { return _parent; }
	const DrawableObject* getRoot() const { if (getParent() == nullptr) return this; else return getRoot(); }
	int getTag() const { return _tag; }
	virtual void setTag(int tag) { _tag = tag; }
	int getDepth() const { return _depth; }
	virtual void setDepth(int depth) { _depth = depth; }
	void addChild(DrawableObject* object, int tag = 0);
	void removeChild(DrawableObject* object);
	void removeChild(int tag);
	virtual void step(float dt);
	void resortObjects() {
		; // run after step
	}
	bool operator==(const DrawableObject& d) const { return &d == this || _tag == d._tag; } // pointer equal or tag equal
	bool operator!=(const DrawableObject& d) const { return !(d == *this); }
	bool operator<(const DrawableObject& d) const { return _depth < d._depth; } // we sort by depth
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (_parent == nullptr) { // only the root draws
			if (!_sortedObjects.empty()) {
				for (const auto& o : _sortedObjects) if(o->_visiable) o->draw(target, states);
			}
		}
		else self_draw(target, states);
	}
};
namespace std {
	template<> bool less<DrawableObject>::operator()(const DrawableObject& lhs, const DrawableObject& rhs) const { return lhs < rhs; }
	template<> bool less<DrawableObject>::operator()(const DrawableObject* lhs, const DrawableObject* rhs) const { return *lhs < *rhs; }
}


class Room;

class RoomObject : public DrawableObject, public GSprite {
private: // simple movment stuff and physics stuff
	sf::Vector2f _movmentVector;
	sf::Vector2f _gravityVector;
	sf::Vector2f _velocityVector;
	float _gravity;
	float _gravityDirection;
	float _direction;
	float _speed;
	float _image_speed;
	float _current_frame;
public:
	const sf::Vector2f getNextPosition(float dt) const {
		sf::Vector2f pos = getPosition();
		pos += _movmentVector  * dt; // add the movment vector first
		pos += _velocityVector  * dt; // add the gravity acceration
		return pos;
	}
	void setImageSpeed(float a) { _image_speed = a; }
	float getImageSpeed() const { return _image_speed; }
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
	RoomObject* instance_create(float x, float y, int index, int spr_index =-1) {// create object linked to this parrent
		RoomObject* obj = create<RoomObject>(index);
		if(spr_index != -1) obj->setUndertaleSprite(spr_index);
		obj->setPosition(x, y);
		return obj;
	}
private:
	friend class Room;
	Room* _room;
	RoomObject(Room* parent);
	bool _visiable;
protected:
	sf::VertexArray _vertices;
	const sf::Texture* _texture; 
public:
	Room& getRoom() const  { return *_room; }
	bool isVisiable() const { return _visiable; }
	void setVisiable(bool v) { _visiable=v;  }
	virtual void self_step(float dt) override {
		sf::Vector2f pos = getNextPosition(dt);
		if (_gravity != 0.0f) _velocityVector += _gravityVector * dt;// velocity += timestep * acceleration;	

		_current_frame += _image_speed;
		if ((std::abs(_current_frame)+0.01f) >= 1.0f) { // we add some flub there
			int next_frame = (int)std::modf(_current_frame + 0.01f, &_current_frame); // this should work
			setImageIndex(next_frame);
		}
	}
	virtual void self_draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		GSprite::draw(target, states);
	}
};
// simple tilemap from examples
class TileMap : public sf::Drawable
{
	sf::RenderTexture _tiles;
	sf::Vertex _vertices[4];
	const Undertale::Room _room;
	bool _hasTiles;
public:
	TileMap(const Undertale::Room& room);
	void loadRoom(const Undertale::Room& room);
	TileMap(size_t index);
	void loadRoom(size_t index);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Room : public DrawableObject {
	friend class RoomObject;
	struct RoomObjectDepthSort {
		bool operator() (const RoomObject& lhs, const RoomObject& rhs) const {
			return lhs.getDepth() < rhs.getDepth();
		}
		bool operator() (const RoomObject* lhs, const RoomObject* rhs) const {
			return lhs->getDepth() < rhs->getDepth();
		}
	};
	typedef std::unordered_set<std::unique_ptr<RoomObject>> t_objectList;
	typedef t_objectList::iterator t_objectList_it;
	typedef std::set<DrawableObject*> t_sortedObjectList;
	typedef t_sortedObjectList::iterator t_sortedObjectList_it;

	


	// helper for simple game maker objects
	RoomObject* instance_create(float x, float y, int index);

	sf::View _vew;
	bool _objectsChanged;
	float _speed;

public:
	
	float getSpeed() const { return _speed; }
	void setSpeed(float speed) { _speed = speed; }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  {

	}
};