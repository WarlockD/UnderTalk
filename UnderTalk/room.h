#pragma once
#include "Global.h"

#include "enitys.h"

// to many things use the sstep system, so lets make one

class VisablitySettings {
	bool _visible;
	float _depth;
public:
	VisablitySettings() : _visible(true), _depth(0.0f) {}
	void setVisible(bool v) { _visible = v; }
	bool getVisible() const { return _visible; }
	virtual void setDepth(float depth) { _depth = depth; }
	float getDepth() const { return _depth; }
	bool operator<(const VisablitySettings& r) const { return r._depth < _depth; }
	bool operator==(const VisablitySettings& r) const { return _depth == r._depth ;  }
	bool operator!=(const VisablitySettings& r) const { return _depth != r._depth; ; }
};

class MovementSettings : public sf::Transformable {
	sf::Vector2f _movment;
	float _direction;
	sf::Vector2f _speed;
public:
	MovementSettings() = default;
	inline virtual void setDirection(float d) { _movment = CreateMovementVector(_direction = d, _speed); }
	inline virtual void setSpeed(float s) { _movment = CreateMovementVector(_direction, _speed = sf::Vector2f(s, s)); }
	inline virtual void setSpeed(sf::Vector2f s) { _movment = CreateMovementVector(_direction, _speed = s); }
	inline void step(float dt) { setPosition(getPosition() + (_movment * dt)); }
};
#if 0
class MovementHelper : public sf::Transformable, public sf::Drawable {
	sf::IntRect _frames;
	sf::Vector2f _movment;
	float _direction;
	sf::Vector2f _speed;
public:
	inline virtual void step(float dt) { setPosition(getPosition() + _movment); }
	inline virtual void setDirection(float d) { _movment = CreateMovementVector(_direction = d, _speed); }
	inline virtual void setSpeed(float s) { _movment = CreateMovementVector(_direction, _speed = sf::Vector2f(s, s)); }
	inline virtual void setSpeed(sf::Vector2f s) { _movment = CreateMovementVector(_direction, _speed = s); }
	inline virtual sf::Vector2f getSpeed() const { return _speed; }

};
#endif

template<typename T>
struct ListEntry {
	using type = T;
	type *le_next;	/* next element */
	type **le_prev;	/* address of previous next element */
	ListEntry() : le_next(nullptr), le_prev(&le_next) {}
};
template<typename T>
struct ListHead {
	using type = T;
	type *lh_first;	/* first element */
	ListHead() : lh_first(nullptr) {}
};

class RoomSprite : public GSprite, public sf::Drawable, public MovementSettings, public VisablitySettings {
protected:
	float _frameSpeed;
	float _currentFrameTime;
	Room* _room;
	RoomSprite* _parentTrasform;
	ListEntry<RoomSprite> _sorted_list;
	void _insert_sort();
	friend class Room;
	friend class RoomObject;

	RoomSprite() : _room(nullptr) {}
public:
	RoomSprite(Room& room, float depth = 0.0f);
	RoomSprite(Room& room, uint32_t sprite_index, float depth = 0.0f);
	~RoomSprite();
	void setUndertaleSprite(uint32_t index);

	virtual void setDepth(float depth) override;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (valid() && getVisible()) {
			states.texture = getTexture().get();
			states.transform *= getTransform();
			target.draw(getVertices(), getVerticesCount(), getVerticesType(), states);
		}
	}
	void setFrame(const gm::SpriteFrame& frame);
	void nextFrame() {
		if (getImageCount() > 1) {
			size_t index = getImageIndex();
			++index;
			if (index >= getImageCount()) index = 0;
			setImageIndex(index);
		}
	}
	void prevFrame() {
		if (getImageCount() > 1) {
			size_t index = getImageIndex();
			if (index == 0) index = getImageCount() - 1;
			else --index;
			setImageIndex(index);
		}
	}

	void setFrameSpeed(float frameSpeed) { frameSpeed = frameSpeed; _currentFrameTime = 0.0f; }
	float getFrameSpeed() const { return _frameSpeed; }
	virtual void step(float dt);
};
// we use these templates for queue.h so we don't have to taint the 
// header files with all the macros queue.h has



class RoomObject : public RoomSprite  {
private:
	ListEntry<RoomObject> _index_hash;
	
	int _object_flags;
	uint32_t _index;
	RoomObject() = default;
protected:
	gm::Object _object;
	std::list<RoomSprite> _sprites; // extra sprites
	friend class Room;
	RoomObject(Room& room);
public:
	RoomObject(Room& room, gm::Object object);
	RoomObject(Room& room, uint32_t index);
	
	virtual ~RoomObject();
	void removeSelf();
	void setObject(gm::Object obj);
	const gm::Object& getObject() const { return _object; }
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	RoomSprite& createSprite(uint32_t index);
	void removeSprite(uint32_t index);
	void removeSprite(RoomSprite& sprite);
	uint32_t index() const { return _index; }

	RoomSprite& create_sprite(int index);
	virtual void step(float dt) override;
};



class Room : public sf::Drawable {
	enum RoomObjectFlags {
		DYNAMIC_FLAG = 1,
	};
	friend class RoomObject;
	friend class RoomSprite;
	gm::DataWinFile& _file;
	ManagerType _manager;
	// helper for simple game maker objects

	sf::View _view;
	bool _objectsChanged;
	float _speed;
	TileMap _tiles;
	gm::Room _room;
#if 0
	class key {
		uint32_t _index;
		RoomObject* _object;
		bool _owned;
	public:
		key() = default;
		key(const key& copy) = delete;
		key(key&& move) :_index(move._index), _object(move._object), _owned(move._owned) { move._object = nullptr; }
		key(uint32_t i) : _index(i), _object(nullptr) {}
		key(RoomObject* o, bool owned = false) : _index(o->getIndex()), _object(o), _owned(owned) {}
		~key() { if (_object &&_owned) delete _object; }
		uint32_t index() const { return  _index; }
		const RoomObject* object() const { return _object; }
		RoomObject* object() { return _object; }
		bool operator==(const key& v) const {
			return (_object != nullptr && v._object != nullptr && _object == v._object) || index() == v.index();
		}
		bool operator<(const key& v) const { return index() < v.index(); }
	};
	struct hasher {
		size_t operator()(const key& k) const { return k.index(); }
	};
	std::unordered_multiset<key, hasher> _objects;
	std::vector<key> _objects_to_delete;
#endif
	std::unordered_multimap<uint32_t, std::unique_ptr<RoomObject>> _object_map;

	std::vector<ListHead<RoomObject>> _objects;
	std::vector<RoomObject*> _objects_to_delete;
	ListHead<RoomSprite> _sprite_list;

	bool deleteObject(RoomObject* obj);

	void insertObject(RoomObject* obj);
	RoomObject* findObject(RoomObject* obj);
	RoomObject* findObject(uint32_t index);
	const RoomObject* findObject(RoomObject* obj) const;
	const RoomObject* findObject(uint32_t index) const;
public:
	Room(gm::DataWinFile& file);
	~Room();
	const gm::DataWinFile& file() { return _file; }
	void step(float dt);
	sf::View& getView() { return _view; }
	const sf::View& getView() const { return _view; }
	float getSpeed() const { return _speed; }
	void setSpeed(float speed) { _speed = speed; }
	size_t instanceCount(uint32_t index) const;
	bool instanceExists(uint32_t index) const { return findObject(index) != nullptr; }
	void loadRoom(uint32_t index);
	void unloadRoom();
	size_t removeObjects(uint32_t index); // returns count deleted
	bool removeObject(RoomObject& obj);
	bool removeObject(uint32_t index);

	template<typename T>
	typename std::enable_if<std::is_base_of<RoomObject, T>::value && !std::is_same<RoomObject, T>::value, T&>
		instance_create(float x, float y, int index) {
		T* obj = new T(*this);
		_objects.emplace(static_cast<RoomObject*>(obj), true);
		_object_list.clear();
		obj->setPosition(x, y);
		return *obj;
	}
	RoomObject& instance_create(float x, float y, int index);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  override;
};
