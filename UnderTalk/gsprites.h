#pragma once
#include "Global.h"

// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 

class GSpriteFrame : public sf::Drawable {
	sf::Vertex  _vertices[6]; ///< Vertices defining the sprite's geometry, should we use quads?
	struct SpriteFrameCache {
		sf::IntRect rect;
		sf::Texture* texture;
		sf::Vertex  vertices[6];
		~SpriteFrameCache();
	};
	std::shared_ptr<SpriteFrameCache> _frame;
	friend class SpriteFrameCacheHelper;
public:
	GSpriteFrame(const Undertale::SpriteFrame* frame, sf::Color color = sf::Color::White);
	GSpriteFrame()  {}
	void setColor(const sf::Color& color);
	void setFrame(const Undertale::SpriteFrame* frame);
	void insertIntoVertexList(sf::VertexArray& list) const;
	inline void insertIntoVertexList(sf::Vertex* dist) const { std::memcpy(dist, _vertices, sizeof(sf::Vertex) * 6); }

	const sf::IntRect& getTextureRect() const;
	const sf::Texture* getTexture() const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

// Each frame we update the position of movmenet
// if you try to set 
class TimeTransformable : public sf::Transformable {
	sf::Vector2f _movmentVector;
	sf::Vector2f _gravityVector;
	float _velocity;
	sf::Vector2f _gravityDirection;
	float _gravity;
	float _direction;
	sf::Vector2f _speed;
public:
	virtual void moveStep(float dt) {
		sf::Vector2f pos = getNextPosition(dt);
		if (_gravity != 0.0f) {	//update
			_velocity += dt * _gravity; ;// velocity += timestep * acceleration;	
		}
	}
	const sf::Vector2f getNextPosition(float dt) const {
		sf::Vector2f pos = getPosition();
		pos += _movmentVector; // add the movment vector first
		if (_gravity != 0.0f) {	// simple acceleration		
			pos += _gravityDirection * (dt * (_velocity + dt * _gravity / 2));// position += timestep * (velocity + timestep * acceleration / 2);
		}
		return pos;
	}
	void setDirection(float d) { _movmentVector = CreateMovementVector(_direction = d, _speed); }
	void setSpeed(float s) { _movmentVector = CreateMovementVector(_direction, _speed = sf::Vector2f(s, s)); }
	void setGravityDirection(float d) { _gravityDirection = CreateDirectionVector(d); _velocity = 0.0f; }
	void setGravity(float s) { _gravity = s; _velocity = 0.0f; }

	void setHSpeed(float s) { _movmentVector.x = s; }
	void setVSpeed(float s) { _movmentVector.y = s; }

	const sf::Vector2f& getMovement() const { return _movmentVector; }
	sf::Vector2f& getMovement() { return _movmentVector; }
};
// Helper class that updates the movment of bits inside of an array
class TimeTransformableVertexArray : public TimeTransformable {
	sf::VertexArray& _array;
	CC_SYNTHESIZE(size_t, _count, Count);
	CC_SYNTHESIZE(size_t, _index, Index);
public:
	TimeTransformableVertexArray(sf::VertexArray& a) : _array(a), _index(0), _count(a.getVertexCount()) {}
	TimeTransformableVertexArray(sf::VertexArray& a, size_t index) : _array(a), _index(0), _count(6) {} // we only support Triangle quads for now
	TimeTransformableVertexArray(sf::VertexArray& a, size_t index, size_t count) : _array(a), _index(index), _count(count) {}
	virtual void moveStep(float dt) override {
		for (size_t i = _index; i < _count; i++) {
			_array[i].position = getNextPosition(dt);
		}
		TimeTransformable::moveStep(dt);
	}
};

class GSprite : public sf::Drawable, public TimeTransformable {
	const Undertale::Sprite* _sprite;
	int _image_index;
	sf::Color _color;
	GSpriteFrame _frame;///< Texture of the sprite
public:
	GSprite() : _sprite(nullptr), _image_index(0), _color(sf::Color::White) {}
	GSprite(int sprite_index, int image_index = 0): _color(sf::Color::White) { setUndertaleSprite(sprite_index); setImageIndex(0); }
	GSprite(const std::string& name, int image_index = 0) : _color(sf::Color::White) { setUndertaleSprite(name); setImageIndex(0); }
	GSprite(const Undertale::Sprite* sprite, int image_index = 0) :_color(sf::Color::White) { setUndertaleSprite(sprite); setImageIndex(0); }

	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	void setUndertaleSprite(const Undertale::Sprite* sprite);

	void setColor(const sf::Color& color) { _frame.setColor(_color = color); }
	const sf::Color& getColor() const { return _color; }
	sf::Vector2f getSize() const { return sf::Vector2f(_sprite->width()*getScale().x, _sprite->height()*getScale().y); }
	sf::FloatRect getBounds() const { return sf::FloatRect(getPosition(), getSize()); }
	const sf::IntRect& getTextureRect() const { return _frame.getTextureRect(); }
	const sf::Texture* getTexture() const { return _frame.getTexture(); }

	void setImageIndex(int index) { _frame.setFrame(_sprite->frames()->at(_image_index = _sprite ? index % _sprite->frames()->size() : 0)); }
	const char* getName() const { return _sprite->name().c_str(); }
	uint32_t getIndex() const { return _sprite->index(); }
	
	int getImageIndex() const { return _image_index; }
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (_sprite) {
			states.transform *= getTransform();
			target.draw(_frame, states);
		}
	}
};