#pragma once
#include "Global.h"

// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 

class GSpriteFrame  {
	static std::unordered_map <const Undertale::SpriteFrame*, std::weak_ptr<GSpriteFrame>> _frame_cache;
protected:
	sf::Vertex  _vertices[4]; ///< Vertices defining the sprite's geometry, should we use quads?
	sf::IntRect _texRect;
	size_t _texture_index;
	sf::Vector2u _size;
	sf::Vector2u _origin;
	const Undertale::SpriteFrame* _frame;
public:
	GSpriteFrame() : _frame(nullptr) {} // invalid state
	GSpriteFrame(const Undertale::SpriteFrame* frame);
	
	void setFrame(const Undertale::SpriteFrame* frame);
	const Undertale::SpriteFrame* frame() const { return _frame; }
	void insertIntoVertexList(sf::VertexArray& list) const;
	void insertIntoVertexList(std::vector<sf::Vertex>& list, sf::PrimitiveType type) const;


	
	const sf::IntRect& getTextureRect() const { return _texRect; }
	SharedTexture::TextureInfo  getTexture()  { return Undertale::GetTexture(_texture_index, _texRect);  }
	const sf::Vector2u getSize() const { return _size; }
	const sf::Vector2u getOrigin() const { return _origin; }
	const sf::Vertex* vertices() const { return _vertices; }
	const size_t vertices_count() const { return 4; }
	const sf::PrimitiveType vertices_type() const { return sf::TriangleStrip; }
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
	void setColor(const sf::Color& color) {
		for (size_t i = _index; i < _count; i++) _array[i].color = color;
	}
};

class GSprite :  public sf::Sprite {
	const Undertale::Sprite* _sprite;
	SharedTexture::TextureInfo _texture;
	size_t _image_index;
	int _depth;
public:
	int getDepth() const { return _depth; }
	void setDepth(int depth) { _depth = depth; }
	GSprite() : sf::Sprite(), _image_index(0), _depth(0) {}
	GSprite(int sprite_index) : _image_index(0), _depth(0) { setUndertaleSprite(sprite_index); }
	GSprite(int sprite_index, int image_index): _image_index(image_index), _depth(0) { setUndertaleSprite(sprite_index);  }
	GSprite(const std::string& name, int image_index = 0) : _image_index(0), _depth(0) { setUndertaleSprite(name);  }
	GSprite(const Undertale::Sprite* sprite, int image_index = 0) :_image_index(0), _depth(0) { setUndertaleSprite(sprite);  }
	const SharedTexture::TextureInfo& getTextureInfo() const { return _texture; }
	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	void setUndertaleSprite(const Undertale::Sprite* sprite);

	sf::Vector2f getLocalSize() const { return sf::Vector2f(_sprite->width(), _sprite->height()); }
	sf::FloatRect getLocalBounds() const { return sf::FloatRect(_sprite->left(), _sprite->top(), _sprite->left() - _sprite->right(), _sprite->bottom() - _sprite->top()); }
	size_t getImageCount() const { return _sprite->frames()->size(); }
	void setImageIndex(size_t index) { 
		_image_index = index %  getImageCount();  
		auto frame = _sprite->frames()->at(_image_index);
		setOrigin(frame->offset_x, frame->offset_y);
		setTextureRect(sf::IntRect(frame->x, frame->y, frame->width, frame->height));
	}
	const char* getName() const { return _sprite->name().c_str(); }
	uint32_t getIndex() const { return _sprite->index(); }
	sf::Image copyToImage() const { return _texture.copyToImage(); }
	int getImageIndex() const { return _image_index; }
	
};

