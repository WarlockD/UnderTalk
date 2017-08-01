#pragma once
#include "Global.h"

// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 

//  Class used to updating and managing a triangle vertex quad.
/// sub class this with verts

// 7/2017 changed it into a static template so that we can use it in many diffrent sprite systems

struct SpriteVerticesInterface {
	//virtual void setTextureRect(const sf::IntRect& rect, bool setsize = true) = 0;
	//virtual void setColor(const sf::Color& color) = 0;// same with color
	//virtual void setVertexPosition(const sf::Vector2f& offset) = 0;
	//virtual void setVertexPosition(const sf::FloatRect& rect) = 0;
	//virtual void setVertexSize(const sf::Vector2f& size) = 0;
	//virtual void SpriteVertices::set(const sf::FloatRect& rect, const sf::IntRect& textRect, sf::Color color) = 0;
	// run to update verts
	virtual const sf::Vertex* getVertices() const = 0; // entire interface is based on this
	virtual ~SpriteVerticesInterface() {}
	// helpers
	size_t getVerticesCount() const { return 6; } // basied off triangles
	sf::PrimitiveType getVerticesType() const { return sf::PrimitiveType::Triangles; }
	const sf::Color&  getColor() const { return getVertices()[0].color; }
	const sf::Vector2f& TopLeft() const { return getVertices()[0].position; }
	const sf::Vector2f& BottomRight() const { return getVertices()[2].position; }
	inline sf::Vector2f getVertexSize() const { return BottomRight() - TopLeft(); } // size is set by texture rect
	inline bool valid() const { return getVertices() != nullptr; }
	virtual void draw_vertices(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(getVertices(), getVerticesCount(), sf::PrimitiveType::Triangles, states);
	}
};

template<typename T>
class SpriteVerticesHealper : public SpriteVerticesInterface { // : public SpriteVertices {
public:
	// we require below
	using type = std::decay_t<T>;
	void set(const sf::FloatRect& rect, const sf::IntRect& textRect, sf::Color color) {
		sf::Vertex* vertices = getVertices();
		float u1 = static_cast<float>(textRect.left);
		float u2 = u1 + static_cast<float>(textRect.width);
		float v1 = static_cast<float>(textRect.top);
		float v2 = v1 + static_cast<float>(textRect.height);

		float left = rect.left;
		float right = left + rect.width;
		float top = rect.top;
		float bottom = top + rect.height;

		vertices[0] = sf::Vertex(sf::Vector2f(left, top), sf::Color::White, sf::Vector2f(u1, v1));
		vertices[1] = sf::Vertex(sf::Vector2f(left, bottom), sf::Color::White, sf::Vector2f(u1, v2));
		vertices[2] = sf::Vertex(sf::Vector2f(right, bottom), sf::Color::White, sf::Vector2f(u2, v2));

		vertices[3] = sf::Vertex(sf::Vector2f(right, top), sf::Color::White, sf::Vector2f(u2, v1));
		vertices[4] = sf::Vertex(sf::Vector2f(left, bottom), sf::Color::White, sf::Vector2f(u1, v2));
		vertices[5] = sf::Vertex(sf::Vector2f(right, bottom), sf::Color::White, sf::Vector2f(u2, v2));
	}
	void set(const sf::Vector2f& pos, const sf::Color& color) { set(sf::FloatRect(pos, sf::Vector2f(1.0f, 1.0f)), sf::IntRect(0, 0, 1, 1), color); }
	void set(const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color) { set(sf::FloatRect(pos, sf::Vector2f((float)textRect.width, (float)textRect.height)), textRect, color); }
	void set() {} // for var templates with no args

	void setColor(const sf::Color& color) {
		sf::Vertex* vertices = getVertices();
		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;
		vertices[4].color = color;
		vertices[5].color = color;
	}

	void setVertexPosition(const sf::FloatRect& rect) {
		sf::Vertex* vertices = getVertices();
		float left = rect.left;
		float right = left + rect.width;
		float top = rect.top;
		float bottom = top + rect.height;
		vertices[0].position = sf::Vector2f(left, top);
		vertices[1].position = sf::Vector2f(left, bottom);
		vertices[2].position = sf::Vector2f(right, bottom);

		vertices[3].position = sf::Vector2f(right, top);
		vertices[4].position = sf::Vector2f(left, bottom);
		vertices[5].position = sf::Vector2f(right, bottom);
	}
	void setVertexSize(const sf::Vector2f& size) { setVertexPosition(sf::FloatRect(getVertexPosition(), size)); }
	void setVertexPosition(const sf::Vector2f& pos) { setVertexPosition(sf::FloatRect(pos, getVertexSize())); }

	void setTextureRect(const sf::IntRect& rect, bool setsize) {
		sf::Vertex* vertices = getVertices();
		float u1 = static_cast<float>(rect.left);
		float u2 = u1 + static_cast<float>(rect.width);
		float v1 = static_cast<float>(rect.top);
		float v2 = v1 + static_cast<float>(rect.height);
		vertices[0].texCoords = Vector2f(u1, v1);
		vertices[1].texCoords = Vector2f(u1, v2);
		vertices[2].texCoords = Vector2f(u2, v2);

		vertices[3].texCoords = Vector2f(u2, v1);
		vertices[4].texCoords = Vector2f(u1, v2);
		vertices[5].texCoords = Vector2f(u2, v2);
		if (setsize) setVertexSize(sf::Vector2f((float)rect.width, (float)rect.height));
	}
	inline const sf::Vertex* getVertices() const override { return self().getVertices(); }
private:
	friend T;
	inline sf::Vertex* getVertices() { return self().getVertices(); }
	inline type& self() { return static_cast<type&>(*this); }
	inline const type& self() const { return static_cast<const type&>(*this); }
};

class SpriteVerticesConstRef : public SpriteVerticesInterface {
public:
	SpriteVerticesConstRef() : _vertices(nullptr) {}
	SpriteVerticesConstRef(const sf::Vertex* vertices) : _vertices(vertices) {}
	SpriteVerticesConstRef(const SpriteVerticesInterface& vertices) : _vertices(vertices.getVertices()) {}
	virtual ~SpriteVerticesConstRef() {}
	inline const sf::Vertex* getVertices() const override final { return _vertices; }
private:
	const sf::Vertex* _vertices;
};

class SpriteVertices : public SpriteVerticesHealper<SpriteVertices> {
public:
	SpriteVertices() = default;
	SpriteVertices(const SpriteVerticesInterface& i) {
		std::memcpy(_vertices, i.getVertices(), getVerticesCount());
	}
	SpriteVertices(const sf::Vector2f& pos, const sf::Color& color) { set(sf::FloatRect(pos, sf::Vector2f(1.0f, 1.0f)), sf::IntRect(0, 0, 1, 1), color); }
	SpriteVertices(const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color)  { set(sf::FloatRect(pos, sf::Vector2f((float)textRect.width, (float)textRect.height)), textRect, color); }
	SpriteVertices(const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color) { set(posRect, textRect, color); }
	inline const sf::Vertex* getVertices() const override final { return _vertices; }
private:
	friend class SpriteVerticesHealper<SpriteVertices>;
	friend class SpriteVerticesRef;
	inline sf::Vertex* getVertices()  { return _vertices; }
	sf::Vertex _vertices[6];
};

class SpriteVerticesRef : public SpriteVerticesHealper<SpriteVerticesRef> {
public:
	using helper = SpriteVerticesHealper<SpriteVerticesRef>;
	template<typename ... Args>
	SpriteVerticesRef(sf::Vertex* vertices, Args&& ... args) : _vertices(vertices) { set(std::forward<Args>(args)...); }
	SpriteVerticesRef(SpriteVertices& v) : _vertices(v.getVertices()) {}
	virtual ~SpriteVerticesRef() {}
	inline const sf::Vertex* getVertices() const override final { return _vertices; }
private:
	friend class SpriteVerticesHealper<SpriteVerticesRef>;
	inline sf::Vertex* getVertices() { return _vertices; }
	sf::Vertex* _vertices;
};



// unbatched simple sprite
class Sprite : public sf::Transformable, public sf::Drawable, public SpriteVertices {
public:
	Sprite() = default;
	Sprite(const SharedTexture& texture, const SpriteVerticesInterface& i): SpriteVertices(i), _texture(texture) { }
	Sprite(const SharedTexture& texture, const sf::Vector2f& pos, const sf::Color& color) : SpriteVertices(pos, color) , _texture(texture) {}
	Sprite(const SharedTexture& texture, const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color) : SpriteVertices(pos, textRect, color), _texture(texture) {}
	Sprite(const SharedTexture& texture, const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color) : SpriteVertices(posRect,textRect, color), _texture(texture) {}
	Sprite(const SharedTexture& texture, const sf::IntRect& textRect) : SpriteVertices(sf::Vector2f(0.0f,0.0f), textRect, sf::Color::White), _texture(texture) {}
	const SharedTexture& getTexture() const { return _texture; }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		states.texture = getTexture().get();
		states.transform *= getTransform();
		target.draw(getVertices(), getVerticesCount(), getVerticesType(), states);
	}
private:
	SharedTexture _texture;
};