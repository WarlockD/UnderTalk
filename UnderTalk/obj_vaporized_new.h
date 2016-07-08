#pragma once
#include "Global.h"
#include "gsprites.h"

#include <queue>


// fucking retarded, I can get away with 4 points using a triangle strip, arg
class PixelSpriteRef  {
protected:
	sf::Vertex* _vertices;
	int _tag;
	void updateVerts(const sf::FloatRect& rect) {
		_vertices[0].position = sf::Vector2f(rect.left, rect.top);
		_vertices[1].position = sf::Vector2f(rect.left + rect.width, rect.top);
	    _vertices[2].position = sf::Vector2f(rect.left, rect.top + rect.height);

		_vertices[3].position = sf::Vector2f(rect.left, rect.top + rect.height);
		_vertices[4].position = sf::Vector2f(rect.left + rect.width, rect.top);
		_vertices[5].position = sf::Vector2f(rect.left + rect.width, rect.top + rect.top);
	}
public:
	PixelSpriteRef(sf::Vertex* a) : _vertices(a) , _tag(0) {}
	PixelSpriteRef(std::vector<sf::Vertex>& a, size_t index) : _vertices(a.data() + index), _tag(0) {  }
	virtual ~PixelSpriteRef() {}

	const sf::Vector2f& getPosition() const { return _vertices[0].position; }
	sf::Vector2f getSize() const { return _vertices[3].position - _vertices[0].position; }
	sf::FloatRect getBounds() const { return sf::FloatRect(getPosition(), getSize()); }

	void setPosition(const sf::Vector2f& v) { updateVerts(sf::FloatRect(v, getSize())); }
	void setSize(const sf::Vector2f& v) { updateVerts(sf::FloatRect(getPosition(), v)); }
	void setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }
	void setSize(float x, float y) { setSize(sf::Vector2f(x, y)); }
	void move(const sf::Vector2f& v) {
		for (size_t i = 0; i < 6; i++) _vertices[i].position += v;
	}
	void setVertices(sf::Vertex* a) { _vertices = a; }
	void setVertices(std::vector<sf::Vertex>& a, size_t index) { _vertices = a.data() + index; }

	sf::Vertex& getVertices() { return *_vertices; }
	const sf::Vertex& getVertices() const { return *_vertices; }

	void setColor(const sf::Color& c) {
		_vertices[0].color = c;
		_vertices[1].color = c;
		_vertices[2].color = c;
		_vertices[3].color = c;
		_vertices[4].color = c;
		_vertices[5].color = c;
	}
	sf::Color getColor() const { return _vertices[0].color; }
	int getTag() const { return _tag; }
	void setTag(int tag) { _tag = tag; }
	bool operator==(const PixelSpriteRef& other) const { return _vertices == other._vertices; }
	bool operator!=(const PixelSpriteRef& other) const { return _vertices != other._vertices; }
};

// manages its own pixel
class PixelSprite : public PixelSpriteRef, public sf::Drawable {
public:
	PixelSprite();
	~PixelSprite();
	PixelSprite(const PixelSprite& copy);
	PixelSprite(PixelSprite&& move);
	PixelSprite& operator=(const PixelSprite& copy);
	PixelSprite& operator=(PixelSprite&& move);
	PixelSpriteRef toRef() { return PixelSpriteRef(_vertices); }
	// still need to run step
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};



class obj_vaporized_new : public sf::Drawable, public sf::Transformable {
	
	class SimpleDust {
		sf::Vertex* _vertices;
		Rigidbody _body;
		sf::Color _start;
		float _fade;
		void setPosition(float left, float top) {
			_vertices[0].position = sf::Vector2f(left, top);
			_vertices[1].position = sf::Vector2f(left + 1.0f, top);
			_vertices[2].position = sf::Vector2f(left, top + 1.0f);

			_vertices[3].position = sf::Vector2f(left, top + 1.0f);
			_vertices[4].position = sf::Vector2f(left + 1.0f, top);
			_vertices[5].position = sf::Vector2f(left + 1.0f, top + 1.0f);
		}
		void move(const sf::Vector2f& v) {
			for(size_t i=0;i < 6;i++)_vertices[i].position += v;
		}
		void setColor(const sf::Color& c) {
			for (size_t i = 0; i < 6; i++)_vertices[i].color = c;
		}
	public:
		SimpleDust(sf::Vertex* a) : _vertices(a), _body(){
			_body.getVelocity().x = util::random(4.0f) - 2.0f;
			_body.getGravity().y = -util::random(0.5f) - 0.2f;
			_start = a[0].color;
			_fade = 1.0f;
		}

		bool step(float dt) {
			if (_fade < 0.0) return true;
			_fade -= 0.1f;
			sf::Color color = _vertices[0].color;
			color.a = (_start.a * _fade) < 5.0f ? 0 : (sf::Uint8)(_start.a * _fade);
			setColor(color);
			_body.step(dt, _vertices,6);
			return false;
		}
	};
	typedef std::list<SimpleDust> t_active;
	typedef t_active::iterator t_active_it;
	std::vector<sf::Vertex> _vertices;
	std::list<t_active> _lines;
	sf::Vector2f _size;
	int _cooldown;
	
	t_active _active;
public:
	// spec true makes evey pixel independent of one another
	//
	obj_vaporized_new(uint32_t index, bool spec);
	obj_vaporized_new(const GSprite& sprite, bool spec);
	void setPixels(uint32_t index, bool spec);
	void setPixels(const GSprite& sprite, bool spec);
	const sf::Vector2f& getSize() const { return _size; }
	void reset();
	void debugStart() { _cooldown = 0; }
	bool doDustLine();
	void step(float dt);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
