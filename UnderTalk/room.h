#pragma once
#include "Global.h"

#include "node.h"
#include "gsprites.h"

// simple tilemap from examples
class TileMap : public sf::Drawable
{
	sf::RenderTexture _tiles;
	sf::Vertex _vertices[4]; // We draw to a render texture and this is the size of it
	Undertale::Room _room;
	bool _hasTiles;
public:
	TileMap() : _hasTiles(false) {}
	void loadRoom(const Undertale::Room& room);
	void unloadRoom();
	TileMap(size_t index);
	void loadRoom(size_t index);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


class SpriteNode : public Node , public GSpriteFrame {
	Undertale::Sprite _sprite;
	size_t _image_index;
public:
	SpriteNode() : Node(), GSpriteFrame(), _image_index(0) {}
	SpriteNode(int sprite_index) : Node(), _image_index(0) { setUndertaleSprite(sprite_index); }
	SpriteNode(int sprite_index, int image_index) : Node(), _image_index(image_index) { setUndertaleSprite(sprite_index); }
	SpriteNode(const std::string& name, int image_index = 0) : Node(), _image_index(0) { setUndertaleSprite(name); }
	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	const Undertale::Sprite& getUndertaleSprite() const { return _sprite; }

	sf::Vector2f getLocalSize() const { return sf::Vector2f((float)_sprite.width(), (float)_sprite.height()); }
	sf::FloatRect getLocalBounds() const { return sf::FloatRect((float)_sprite.left(), (float)_sprite.top(), (float)_sprite.left() - (float)_sprite.right(), (float)_sprite.bottom() - (float)_sprite.top()); }
	size_t getImageCount() const { return _sprite.valid() ?  _sprite.frames().size() : 0; }
	void setImageIndex(int index);
	const char* getName() const { return _sprite.name().c_str(); }
	uint32_t getIndex() const { return _sprite.index(); }
	int getImageIndex() const { return _image_index; }
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


class RoomObject : public Node {
private: // simple movment stuff and physics stuff
	SpriteNode _sprite;
	bool _visiable;
	float _image_speed;
	float _current_frame;
public:
	RoomObject();
	void setUndertaleSprite(int index) { 
		if (index == -1 && _sprite.getParent() != nullptr) {
			removeChild(&_sprite);
			setSize(sf::Vector2f(0.0f, 0.0f));
		}
		else {
			_sprite.setUndertaleSprite(index);
			if (_sprite.getParent() == nullptr) addChild(&_sprite);
			setSize(_sprite.getSize());
		}
	}
	void setImageSpeed(float a) { _image_speed = a; }
	float getImageSpeed() const { return _image_speed; }
	bool isVisiable() const { return _visiable; }
	void setVisiable(bool v) { _visiable=v;  }
	virtual void step(float dt); // We do run step on all the children...
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override; // .. we don't run draw on any of the children though
};

class Room : public Node {
	friend class RoomObject;

	// helper for simple game maker objects
	RoomObject* instance_create(float x, float y, int index);
	sf::View _view;
	bool _objectsChanged;
	float _speed;
	TileMap _tiles;
	Undertale::Room _room;
public:
	Room() : Node(), _view(sf::FloatRect(0.0f, 0.0f, 640.0f, 480.0f)) {}
	sf::View& getView() { return _view; }
	const sf::View& getView() const { return _view; }
	float getSpeed() const { return _speed; }
	void setSpeed(float speed) { _speed = speed; }
	void loadRoom(int index);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  override {
		target.setView(_view);
		target.draw(_tiles, states);
		Node::draw(target, states);
	}
};