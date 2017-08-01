#pragma once
#include "Global.h"
#include "VertexHelper.h"

class GSpriteFrame  {
protected:
//	const gm::SpriteFrame* _frame; // to save space this could just be a pointer
	sf::Vertex  _vertices[4]; ///< Vertices defining the sprite's geometry, should we use quads?
	SharedTexture _texture;
	sf::IntRect _texRect;
	sf::Vector2u _size;
	sf::Vector2u _origin;

	void updateVertices(const gm::SpriteFrame& frame);
public:
	GSpriteFrame() :_texture(nullptr) {}
	GSpriteFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame);
	void insertIntoVertexList(sf::VertexArray& list) const;
	void insertIntoVertexList(std::vector<sf::Vertex>& list, sf::PrimitiveType type) const;
	void setFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame);
	const sf::IntRect& getTextureRect() const { return _texRect; }
	SharedTexture getTexture() const { return _texture;  }
	const sf::Vector2f getFrameSize() const { return sf::Vector2f((float)_size.x,(float)_size.y); }
	const sf::Vector2u getOrigin() const { return _origin; }
	const sf::Vertex* getVertices() const { return _vertices; }
	const size_t getVerticesCount() const { return 4; }
	const sf::PrimitiveType getVerticesType() const { return sf::TriangleStrip; }
	bool valid() const { return  _texture != nullptr; }
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
	FontCache() = default;
	FontCache(const gm::DataWinFile& file, gm::Font font);
//	FontCache(gm::DataWinFile& file, uint32_t index) : FontCache(file, file.resource_at<gm::Font>(index)) {}
	SpriteVerticesConstRef vglyph_at(size_t index) const;
	sf::Glyph glyph_at(size_t index) const;

	size_t font_size() const { return _font.size(); }
	const gm::StringView& name() const { return _font.name(); }
	uint32_t index() const { return _font.index(); }
	bool valid() const { return _font.valid(); }
	const SharedTexture& getTexture() const { return _texture; }
private:
	gm::Font _font;
	sf::IntRect _textureFrame;
	std::unordered_map<size_t, SpriteVertices> _glyph_vertices;
	std::unordered_map<size_t, sf::Glyph> _glyphs;
	SharedTexture _texture;
};