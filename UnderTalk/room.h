#pragma once
#include "Global.h"

#include "enitys.h"





class Room  : public sf::Drawable {
	friend class RoomObject;

	ManagerType _manager;
	// helper for simple game maker objects
	
	sf::View _view;
	bool _objectsChanged;
	float _speed;
	TileMap _tiles;
	Undertale::Room _room;
	std::vector<entity_t> _enitys; // sorted list
	std::unordered_multimap<int, entity_t> _enitys;
public:
	Room() : _view(sf::FloatRect(0.0f, 0.0f, 640.0f, 480.0f)) {}
	void step(float dt);
	sf::View& getView() { return _view; }
	const sf::View& getView() const { return _view; }
	float getSpeed() const { return _speed; }
	void setSpeed(float speed) { _speed = speed; }
	void loadRoom(int index);
	entity_t instance_create(float x, float y, int index) {
		entity_t obj = _manager.create_entity();
		PositionComponent p;
		p.setPosition(x, y);
		obj.add_component(p);
	}
#if 0
	RoomObject* Room::instance_create(float x, float y, int index) {
		RoomObject* obj = new RoomObject(); // auto adds to the child system
		obj->setParent(this);
		obj->setPosition(x, y);
		obj->setTag(index); // index is the tag
		return obj;
	}
#endif
	template<typename T, typename ... Args, typename = std::enable_if<std::is_base_of<RoomObject,T>::value>>
	T* instance_create(float x, float y, Args&& ... args) {
		T* obj = new T(); // auto adds to the child system

		obj->setParent(this);
		obj->setPosition(x, y);
		obj->setTag(index); // index is the tag
		obj->on_create(std::forward<Args>(args)...);
		return obj;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  override {
		target.setView(_view);
		target.draw(_tiles, states);
		Node::draw(target, states);
	}
};

