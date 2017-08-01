#include "gsprites.h"
using namespace sf;


FontCache::FontCache(const gm::DataWinFile& file, gm::Font font) :_font(font) {
	_textureFrame = IntRect(font.frame().x, font.frame().y, font.frame().width, font.frame().height);
	_texture = Undertale::GetTexture(file,font.frame().texture_index);
	for (auto& glyph : font.glyphs()) {
		sf::Glyph g;
		g.advance = glyph.shift;
		auto rect = sf::IntRect(glyph.x, glyph.y, glyph.width, glyph.height);
		// glyph["offset"] // kind of important, but not sure where it goes
		g.textureRect = rect;
		g.bounds = FloatRect((float)0, (float)0, (float)rect.width, (float)rect.height);
		g.textureRect.top += _textureFrame.top;
		g.textureRect.left += _textureFrame.left;
		// g.bounds ignore for now
		_glyphs.emplace(std::make_pair(static_cast<size_t>(glyph.ch), g));
	}
}

 sf::Glyph FontCache::glyph_at(size_t index) const {
	auto it = _glyphs.find(index);
	//if (it == _glyph_vertices.end()) return SpriteVerticesConstRef();
	return it->second;
}
 SpriteVerticesConstRef FontCache::vglyph_at(size_t index) const {
	auto it = _glyph_vertices.find(index);
	if (it == _glyph_vertices.end()) return SpriteVerticesConstRef();
	else return it->second;
}

std::unordered_map<size_t, SpriteVertices> _glyph_vertices;
std::unordered_map<size_t, sf::Glyph> _glyphs;
//GSpriteFrame* cframe = new GSpriteFrame;
//cframe->_texRect = IntRect(frame->x, frame->y, frame->width, frame->height);


void GSpriteFrame::updateVertices(const gm::SpriteFrame& frame) {
	_texRect = IntRect(frame.x, frame.y, frame.width, frame.height);
	_origin = Vector2u(frame.offset_x, frame.offset_y);
	{
		float width = static_cast<float>(_texRect.width);
		float height = static_cast<float>(_texRect.height);

		float u1 = static_cast<float>(_texRect.left);
		float u2 = u1 + width;
		float v1 = static_cast<float>(_texRect.top);
		float v2 = v1 + height;

		float left = static_cast<float>(_origin.x);
		float right = left + width;
		float top = static_cast<float>(_origin.y);
		float bottom = top + height;

		_vertices[0] = Vertex(Vector2f(left, top), Color::White, Vector2f(u1, v1));
		_vertices[1] = Vertex(Vector2f(left, bottom), Color::White, Vector2f(u1, v2));
		_vertices[2] = Vertex(Vector2f(right, top), Color::White, Vector2f(u2, v1));
		_vertices[3] = Vertex(Vector2f(right, bottom), Color::White, Vector2f(u2, v2));
	}
}

GSpriteFrame::GSpriteFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame)  {
	updateVertices(frame);
	_texture = Undertale::GetTexture(file, frame.texture_index);
}

void GSpriteFrame::setFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame) {
	updateVertices(frame);
	_texture = Undertale::GetTexture(file, frame.texture_index);
}


void GSpriteFrame::insertIntoVertexList(sf::VertexArray& list) const {
	auto type = list.getPrimitiveType();
	if (type == sf::PrimitiveType::Triangles) {
		list.append(_vertices[0]);
		list.append(_vertices[1]);
		list.append(_vertices[3]);

		list.append(_vertices[2]);
		list.append(_vertices[1]);
		list.append(_vertices[3]);

	}
	else if (type == PrimitiveType::TrianglesStrip) {
		list.append(_vertices[0]);
		list.append(_vertices[1]);
		list.append(_vertices[2]);
		list.append(_vertices[3]);
	}
	else throw std::exception("Bad type to insert");
}

void GSpriteFrame::insertIntoVertexList(std::vector<sf::Vertex>& list, sf::PrimitiveType type) const {
	if (type == sf::PrimitiveType::Triangles) {
		list.push_back(_vertices[0]);
		list.push_back(_vertices[1]);
		list.push_back(_vertices[3]);

		list.push_back(_vertices[2]);
		list.push_back(_vertices[1]);
		list.push_back(_vertices[3]);

	}
	else if (type == PrimitiveType::TrianglesStrip) {
		list.push_back(_vertices[0]);
		list.push_back(_vertices[1]);
		list.push_back(_vertices[2]);
		list.push_back(_vertices[3]);
	}
	else throw std::exception("Bad type to insert");
}

#if 0
void GSprite::setUndertaleSprite(int index) {
	if (index == -1) {
		_sprite = gm::Sprite();
		setFrame(gm::SpriteFrame());
	}
	else {
		_sprite = GetUndertale().LookupSprite(index);
		setImageIndex(0);
	}
}
void GSprite::setUndertaleSprite(const std::string& name) {
	assert(false);
}
#endif
void GSprite::setImageIndex(int index) {
	assert(valid());
	_image_index = std::abs(index %  (int)getImageCount());
	const auto& frame = _sprite.frames().at(_image_index);
	updateVertices(frame);
}
void GSprite::setImageIndex(int index, gm::DataWinFile& file) {
	_image_index = std::abs(index % (int)getImageCount());
	const auto& frame = _sprite.frames().at(_image_index);
	_texture = Undertale::GetTexture(file, frame.texture_index);
	updateVertices(frame);
}

