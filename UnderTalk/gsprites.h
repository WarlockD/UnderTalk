#pragma once
#include "Global.h"
#include "VertexHelper.h"
// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 

//  Class used to updating and managing a triangle vertex quad.
/// sub class this with verts
class SpriteVertices {
protected:
	sf::Vertex* _vertices;
	bool _owned;
	virtual void draw_vertices(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(getVertices(), 6, sf::PrimitiveType::Triangles, states);
	}
public:
	sf::Vertex* getVertices() { return _vertices; }
	const sf::Vertex* getVertices() const { return _vertices; }
	SpriteVertices();
	SpriteVertices(const sf::Vector2f& pos, const sf::Color& color = sf::Color::White);
	SpriteVertices(const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color = sf::Color::White);
	SpriteVertices(const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color = sf::Color::White);
	SpriteVertices(sf::Vertex* vertices);
	SpriteVertices(sf::Vertex* vertices, const sf::Vector2f& pos, const sf::Color& color = sf::Color::White);
	SpriteVertices(sf::Vertex* vertices, const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color = sf::Color::White);
	SpriteVertices(sf::Vertex* vertices, const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color = sf::Color::White);
	/// rule of 5 cause we have a managed pointer! maybe!
	// Carful with copy, it will create a new vertex, copy the original and the copy will be managed it does NOT move the refrence
	SpriteVertices(const SpriteVertices& copy);
	SpriteVertices& operator=(const SpriteVertices& copy);
	SpriteVertices(SpriteVertices&& move);
	SpriteVertices& operator=(SpriteVertices&& move);
	~SpriteVertices();


	void set(const sf::FloatRect& rect,  const sf::IntRect& textRect, sf::Color color = sf::Color::White); // main that gets all piped into

	/// Normaly you want setsize as it will keep the vert aspect ratio correct
	void setTextureRect(const sf::IntRect& rect, bool setsize=true); 
	void setColor(const sf::Color& color); // same with color
	void setVertexPosition(const sf::Vector2f& offset);
	void setVertexPosition(const sf::FloatRect& rect);
	void setVertexSize(const sf::Vector2f& size);
	
	// run to update verts
	const sf::Vector2f& getVertexPosition() const { return _vertices[0].position; }
	const sf::Color&  getColor() const { return _vertices[0].color; }
	sf::Vector2f getVertexSize() const;
};

class GSpriteFrame  {
protected:
	const gm::SpriteFrame* _frame; // to save space this could just be a pointer
	sf::Vertex  _vertices[4]; ///< Vertices defining the sprite's geometry, should we use quads?
	SharedTexture _texture;
	sf::IntRect _texRect;
	sf::Vector2u _size;
	sf::Vector2u _origin;

	void updateVertices();
	void updateTexture(gm::DataWinFile& file);
public:
	GSpriteFrame() : _frame(nullptr) {}
	GSpriteFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame);
	void insertIntoVertexList(sf::VertexArray& list) const;
	void insertIntoVertexList(std::vector<sf::Vertex>& list, sf::PrimitiveType type) const;
	void setFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame);
	const gm::SpriteFrame& getFrame() const { return *_frame; }
	
	const sf::IntRect& getTextureRect() const { return _texRect; }
	SharedTexture getTexture() const { return _texture;  }
	const sf::Vector2f getFrameSize() const { return sf::Vector2f((float)_size.x,(float)_size.y); }
	const sf::Vector2u getOrigin() const { return _origin; }
	const sf::Vertex* getVertices() const { return _vertices; }
	const size_t getVerticesCount() const { return 4; }
	const sf::PrimitiveType getVerticesType() const { return sf::TriangleStrip; }
	bool valid() const { return _frame != nullptr && _texture; }
};


class GSprite :  public GSpriteFrame {
protected:
	gm::Sprite _sprite;
	size_t _image_index;
public:
	GSprite() : GSpriteFrame(), _image_index(0) {}
	GSprite(gm::DataWinFile& file, int sprite_index) : _image_index(0) { setUndertaleSprite(file,sprite_index); }
	GSprite(gm::DataWinFile& file, int sprite_index, int image_index): _image_index(image_index) { setUndertaleSprite(file,sprite_index);  }
	GSprite(gm::DataWinFile& file, const std::string& name, int image_index = 0) : _image_index(0) { setUndertaleSprite(file,name);  }
	void setUndertaleSprite(gm::DataWinFile& file, int index) {
		if (index == -1) {
			_sprite = gm::Sprite();
		}
		else {
			_sprite = file.resource_at<gm::Sprite>(index);
			setImageIndex(0);
		}
	}
	void setUndertaleSprite(gm::DataWinFile& file, const std::string& name) {
		assert(0);
		// not supported yet
	}
	const gm::Sprite& getUndertaleSprite() const { return _sprite; }

	sf::Vector2f getLocalSize() const { return sf::Vector2f((float)_sprite.width(), (float)_sprite.height()); }
	sf::FloatRect getLocalBounds() const { return sf::FloatRect((float)_sprite.left(), (float)_sprite.top(), (float)_sprite.left() - (float)_sprite.right(), (float)_sprite.bottom() - (float)_sprite.top()); }
	size_t getImageCount() const { return _sprite.frames().size(); }
	void setImageIndex(int index);
	void setImageIndex(int index, gm::DataWinFile& file);
	const gm::StringView& getName() const { return _sprite.name(); }
	uint32_t getIndex() const { return _sprite.index(); }
	size_t getImageIndex() const { return _image_index; }
	bool valid() const { return _sprite.valid(); }
};
class FontCache {

public:
	class GlyphSprite : public sf::Transformable, public sf::Drawable {
		sf::Vertex  _vertices[4]; ///< Vertices defining the sprite's geometry, should we use quads?
	public:
		GlyphSprite() = default;
		GlyphSprite(gm::Font::Glyph glyph);
		const sf::Vertex* getVertices() const { return _vertices; }
		size_t size() const { return 4; }
	};
	FontCache(gm::DataWinFile& file, gm::Font font) :_font(font) {}

	const GlyphVerts& glyph_at(size_t index) const {
		auto it = _glyphs.find(index);
		if (it != _glyphs.end()) {
			GlyphVerts(_font.glyph_at(index)
		}
		return it->second;
	}
	bool valid() const { return _font.valid(); }
private:
	gm::Font _font;
	mutable std::unordered_map<size_t, GlyphSprite> _glyphs;
	SharedTexture _texture;
};