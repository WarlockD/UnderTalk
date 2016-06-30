#pragma once
#include "Global.h"

// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 

class GSpriteFrame : public sf::Drawable {
	sf::Vertex _vertices[6]; ///< Vertices defining the sprite's geometry, should we use quads?
	const sf::Texture* _texture;     ///< Texture of the sprite
	sf::IntRect _textureRect; ///< Rectangle defining the area of the source texture to display
		// This structure is pointed to somewhere in the data win
public:
	GSpriteFrame(const Undertale::SpriteFrame& frame, sf::Color color = sf::Color::White);
	GSpriteFrame() : _texture(nullptr) {}
	void setColor(const sf::Color& color);
	void setFrame(const Undertale::SpriteFrame& frame);
	const sf::Texture& getTexture() const { return *_texture; }
	void insertIntoVertexList(sf::VertexArray& list) const;
	inline void insertIntoVertexList(sf::Vertex* dist) const { std::memcpy(dist, _vertices, sizeof(sf::Vertex) * 6); }
	inline void draw(sf::RenderTarget& target, sf::RenderStates states) const { 
		if (_texture) {
			states.texture = _texture;  
			target.draw(_vertices, 6, sf::PrimitiveType::Triangles, states);
		}
	}
};


class GSprite : public sf::Drawable, public sf::Transformable {
	const Undertale::Sprite* _sprite;
	int _image_index;
	sf::Color _color;
	const sf::Texture* _texture;     ///< Texture of the sprite
	std::vector<sf::Vertex> _vertices; // vertexes of all frames, its in triangles
public:
	GSprite() : _sprite(nullptr), _image_index(0), _color(sf::Color::White), _texture(nullptr) {}
	GSprite(int sprite_index, int image_index = 0): _color(sf::Color::White) { setUndertaleSprite(sprite_index); setImageIndex(0); }
	GSprite(const std::string& name, int image_index = 0) : _color(sf::Color::White) { setUndertaleSprite(name); setImageIndex(0); }
	GSprite(const Undertale::Sprite* sprite, int image_index = 0) :_color(sf::Color::White) { setUndertaleSprite(sprite); setImageIndex(0); }
	inline void insertIntoVertexList(sf::Vertex* dist) const { std::memcpy(dist, _vertices.data(), sizeof(sf::Vertex) * 6); }
	void insertIntoVertexList(sf::VertexArray& list) const;

	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	void setUndertaleSprite(const Undertale::Sprite* sprite);

	void setColor(const sf::Color& color);
	const sf::Color& getColor() const { return _color; }


	void setImageIndex(int index) { _image_index = _sprite ? index % _sprite->frames()->size() : 0; }
	const char* getName() const { return _sprite->name().c_str(); }
	uint32_t getIndex() const { return _sprite->index(); }
	
	int getImageIndex() const { return _image_index; }
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (_sprite) {
			states.texture = _texture;
			states.transform *= getTransform();
			target.draw(_vertices.data() + (_image_index * 6), 6, sf::PrimitiveType::Triangles, states);
		}
	}
};