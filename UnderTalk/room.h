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


class SpriteNode : public Ref , public GSpriteFrame {
	Undertale::Sprite _sprite;
	size_t _image_index;
public:
	SpriteNode() : Ref(), GSpriteFrame(), _image_index(0) {}
	SpriteNode(int sprite_index) : Ref(), _image_index(0) { setUndertaleSprite(sprite_index); }
	SpriteNode(int sprite_index, int image_index) : Ref(), _image_index(image_index) { setUndertaleSprite(sprite_index); }
	SpriteNode(const std::string& name, int image_index = 0) : Ref(), _image_index(0) { setUndertaleSprite(name); }
	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	const Undertale::Sprite& getUndertaleSprite() const { return _sprite; }

	sf::Vector2f getLocalSize() const { return sf::Vector2f((float)_sprite.width(), (float)_sprite.height()); }
	sf::FloatRect getLocalBounds() const { return sf::FloatRect((float)_sprite.left(), (float)_sprite.top(), (float)_sprite.left() - (float)_sprite.right(), (float)_sprite.bottom() - (float)_sprite.top()); }
	size_t getImageCount() const { return _sprite.frames().size(); }
	void setImageIndex(int index);
	const char* getName() const { return _sprite.name().c_str(); }
	uint32_t getIndex() const { return _sprite.index(); }
	int getImageIndex() const { return _image_index; }
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


class RoomObject : public Node,  public sf::Transformable {
private: // simple movment stuff and physics stuff
	SpriteNode _sprite;
	bool _visiable;
	sf::Vector2f _movmentVector;
	sf::Vector2f _gravityVector;
	sf::Vector2f _velocityVector;
	float _gravity;
	float _gravityDirection;
	float _direction;
	float _speed;
	float _image_speed;
	float _current_frame;
	sf::Vector2f _size;
public:
	RoomObject();
	void setUndertaleSprite(int index) { 
		if (index == -1 && _sprite.getParent() != nullptr) 
			removeChild(&_sprite);
		else {
			_sprite.setUndertaleSprite(index);
			if (_sprite.getParent() == nullptr) addChild(&_sprite);
			_size = _sprite.getSize();
		}
	}
	const sf::Vector2f& getSize() const { return _size; }
	void setSize(const sf::Vector2f& size)  { _size=size; }
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
	bool isVisiable() const { return _visiable; }
	void setVisiable(bool v) { _visiable=v;  }
	virtual void step(float dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class Room : public Node {
	friend class RoomObject;

	// helper for simple game maker objects
	RoomObject* instance_create(float x, float y, int index);
	sf::View _view;
	bool _objectsChanged;
	float _speed;
	TileMap _tiles;
public:
	Room() : Node(), _view(sf::FloatRect(0.0f, 0.0f, 640.0f, 480.0f)) {}
	sf::View& getView() { return _view; }
	const sf::View& getView() const { return _view; }
	float getSpeed() const { return _speed; }
	void setSpeed(float speed) { _speed = speed; }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const  override {
		target.setView(_view);
		target.draw(_tiles, states);
		Node::draw(target, states);
	}
};