#pragma once
#include "Global.h"
#include "gsprites.h"

#include <queue>

class VertexSprite{// : public sf::Transformable {
	sf::Vertex* _vertices;
	size_t _size;
public:
	VertexSprite(sf::Vertex* vertices, size_t size) : _vertices(vertices), _size(size) {}
	//void update() { const auto& t = getTransform();  for (size_t i = 0; i < _size; i++) _vertices[i].position = t.transformPoint(_vertices[i].position); }
	void setColor(const sf::Color& color) { if (_vertices != nullptr) for (size_t i = 0; i < _size; i++) _vertices[i].color = color; }
	sf::Color getColor() const { return _vertices != nullptr ? _vertices[0].color : sf::Color(); }
	sf::Vector2f getPosition() { return _vertices[0].position; }

	void setPosition(const sf::Vector2f&v) {
		float left = v.x;
		float top = v.y;
		float right = v.x + 1;
		float bottom = v.y + 1;
		_vertices[0].position = sf::Vector2f(left, top);
		_vertices[1].position = sf::Vector2f(right, top);
		_vertices[2].position = sf::Vector2f(left, bottom);
		_vertices[3].position = sf::Vector2f(left, bottom);
		_vertices[4].position = sf::Vector2f(right, top);
		_vertices[5].position = sf::Vector2f(right, bottom);
	}
};

class PixelArray : public sf::Transformable, public sf::Drawable {
private:
	std::vector<sf::Vertex> _vertices;
	size_t _width;
	size_t _height;
	const sf::Texture* _pixelTexture;
	friend class PixelSprite;
public:
	PixelArray() : _width(0), _height(0), _pixelTexture(nullptr) {}
	PixelArray(size_t width, size_t height);
	PixelArray(const sf::Image& image);
	PixelArray(const sf::Image& image, const sf::IntRect& rect);
	PixelArray(const sf::Texture& texture);
	PixelArray(const sf::Texture& texture, const sf::IntRect& rect);

	void resize(size_t width, size_t height);
	void resize(const sf::Vector2i& size);
	void resize(const sf::Vector2u& size);

	void setPixels(const sf::Image& image);
	void setPixels(const sf::Image& image, const sf::IntRect& rect);
	void setPixels(const sf::Texture& texture);
	void setPixels(const sf::Texture& texture, const sf::IntRect& rect);

	size_t width() const { return _width; }
	size_t height() const { return _height; }
	VertexSprite at(size_t x, size_t y);
	VertexSprite lineAt(size_t y);
	std::vector<VertexSprite> allFilledAt(size_t y);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		states.texture = _pixelTexture;
		states.transform *= getTransform();
		target.draw(_vertices.data(), _vertices.size(), sf::PrimitiveType::Triangles, states);
	}
};
class obj_vaporized_new : public sf::Drawable, public sf::Transformable {
	PixelArray _pixels;
	std::list<RemoveIfFalseAction> _currentActions;
	int _cooldown;
	bool _doStep;
public:
	// spec true makes evey pixel independent of one another
	//
	void start() { _cooldown = 0; }
//	obj_vaporized_new(uint32_t index,bool spec);
	obj_vaporized_new(const GSprite& sprite, bool spec);
	void step(float dt);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
