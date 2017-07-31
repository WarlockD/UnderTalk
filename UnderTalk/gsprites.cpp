#include "gsprites.h"
using namespace sf;




//GSpriteFrame* cframe = new GSpriteFrame;
//cframe->_texRect = IntRect(frame->x, frame->y, frame->width, frame->height);
void GSpriteFrame::updateVertices() {
	_texRect = IntRect(_frame->x, _frame->y, _frame->width, _frame->height);
	_origin = Vector2u(_frame->offset_x, _frame->offset_y);
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

void GSpriteFrame::updateTexture(gm::DataWinFile& file) {
	if (_texture && _texture->texture_index() == _frame->texture_index) return;
	_texture = Undertale::GetTexture(file, _frame->texture_index);
}


SpriteVertices::SpriteVertices() : _vertices(nullptr), _owned(false) {  }
SpriteVertices::SpriteVertices(const sf::Vector2f& pos, const sf::Color& color) : _vertices(new sf::Vertex[6]), _owned(true) { set(FloatRect(pos, Vector2f(1.0f,1.0f)), IntRect(0, 0, 1, 1), color); }
SpriteVertices::SpriteVertices( const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color) : _vertices(new sf::Vertex[6]), _owned(true) { set(FloatRect(pos, Vector2f((float)textRect.width, (float)textRect.height)), textRect, color); }
SpriteVertices::SpriteVertices( const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color) : _vertices(new sf::Vertex[6]), _owned(true) { set(posRect, textRect, color); }

SpriteVertices::SpriteVertices(sf::Vertex* vertices) : _vertices(vertices), _owned(false) {  }
SpriteVertices::SpriteVertices(sf::Vertex* vertices, const sf::Vector2f& pos, const sf::Color& color) : _vertices(vertices), _owned(false) { set(FloatRect(pos, Vector2f(1.0f, 1.0f)), IntRect(0, 0, 1, 1), color); }
SpriteVertices::SpriteVertices(sf::Vertex* vertices, const sf::Vector2f& pos, const sf::IntRect& textRect, const sf::Color& color) : _vertices(vertices), _owned(false) { set(FloatRect(pos, Vector2f((float)textRect.width, (float)textRect.height)), textRect, color); }
SpriteVertices::SpriteVertices(sf::Vertex* vertices, const sf::FloatRect& posRect, const sf::IntRect& textRect, const sf::Color& color ) : _vertices(vertices), _owned(false) { set(posRect, textRect, color); }


SpriteVertices::SpriteVertices(const SpriteVertices& copy) : _vertices(new sf::Vertex[6]), _owned(true) { std::memcpy(copy._vertices, _vertices, sizeof(Vertex) * 6); }
SpriteVertices& SpriteVertices::operator=(const SpriteVertices& copy) {
	if (_vertices == nullptr) { _vertices = new Vertex[6]; _owned = true; }
	std::memcpy(copy._vertices, _vertices, sizeof(Vertex) * 6);
	return *this;
}
SpriteVertices::SpriteVertices(SpriteVertices&& move) : _vertices(move._vertices), _owned(move._owned) { move._vertices = nullptr; }
SpriteVertices& SpriteVertices::operator=(SpriteVertices&& move) {
	if (_owned && _vertices) delete[] _vertices;
	_vertices = move._vertices;
	_owned = move._owned;
	move._vertices = nullptr;
	move._owned = false;
	return *this;
}
SpriteVertices::~SpriteVertices() {
	if (_vertices != nullptr && _owned == true) delete[] _vertices;
	_vertices = nullptr;
}

void SpriteVertices::set(const sf::FloatRect& rect, const sf::IntRect& textRect, sf::Color color) {
	if (_vertices == nullptr) { _vertices = new Vertex[6]; _owned = true; }
	float u1 = static_cast<float>(textRect.left);
	float u2 = u1 + static_cast<float>(textRect.width);
	float v1 = static_cast<float>(textRect.top);
	float v2 = v1 + static_cast<float>(textRect.height);

	float left = rect.left;
	float right = left + rect.width;
	float top = rect.top;
	float bottom = top + rect.height;

	_vertices[0] = Vertex(Vector2f(left, top), Color::White, Vector2f(u1, v1));
	_vertices[1] = Vertex(Vector2f(left, bottom), Color::White, Vector2f(u1, v2));
	_vertices[2] = Vertex(Vector2f(right, bottom), Color::White, Vector2f(u2, v2));

	_vertices[3] = Vertex(Vector2f(right, top), Color::White, Vector2f(u2, v1));
	_vertices[4] = Vertex(Vector2f(left, bottom), Color::White, Vector2f(u1, v2));
	_vertices[5] = Vertex(Vector2f(right, bottom), Color::White, Vector2f(u2, v2));
}

void SpriteVertices::setColor(const sf::Color& color) {
	if (_vertices == nullptr) { _vertices = new Vertex[6]; _owned = true; }
	_vertices[0].color = color;
	_vertices[1].color = color;
	_vertices[2].color = color;
	_vertices[3].color = color;
	_vertices[4].color = color;
	_vertices[5].color = color;
}

void SpriteVertices::setVertexPosition(const sf::FloatRect& rect) {
	if (_vertices == nullptr) { _vertices = new Vertex[6]; _owned = true; }
	float left = rect.left;
	float right = left + rect.width;
	float top = rect.top;
	float bottom = top + rect.height;
	_vertices[0].position = Vector2f(left, top);
	_vertices[1].position = Vector2f(left, bottom);
	_vertices[2].position = Vector2f(right, bottom);

	_vertices[3].position = Vector2f(right, top);
	_vertices[4].position = Vector2f(left, bottom);
	_vertices[5].position = Vector2f(right, bottom);
}
void SpriteVertices::setVertexSize(const sf::Vector2f& size) { setVertexPosition(sf::FloatRect(getVertexPosition(), size)); }
void SpriteVertices::setVertexPosition(const sf::Vector2f& pos) { setVertexPosition(sf::FloatRect(pos, getVertexSize())); }

void SpriteVertices::setTextureRect(const sf::IntRect& rect, bool setsize) {
	if (_vertices == nullptr) { _vertices = new Vertex[6]; _owned = true; }
	float u1 = static_cast<float>(rect.left);
	float u2 = u1 + static_cast<float>(rect.width);
	float v1 = static_cast<float>(rect.top);
	float v2 = v1 + static_cast<float>(rect.height);
	_vertices[0].texCoords = Vector2f(u1, v1);
	_vertices[1].texCoords = Vector2f(u1, v2);
	_vertices[2].texCoords = Vector2f(u2, v2);

	_vertices[3].texCoords = Vector2f(u2, v1);
	_vertices[4].texCoords = Vector2f(u1, v2);
	_vertices[5].texCoords = Vector2f(u2, v2);
	if (setsize) setVertexSize(Vector2f((float)rect.width, (float)rect.height));
}
sf::Vector2f SpriteVertices::getVertexSize() const { return _vertices[2].position - _vertices[0].position; } // size is set by texture rect

GSpriteFrame::GSpriteFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame) : _frame(&frame) {
	updateVertices();
	updateTexture(file);
}

void GSpriteFrame::setFrame(gm::DataWinFile& file, const gm::SpriteFrame& frame) {
	if (_frame != &frame) {
		_frame = &frame;
		updateVertices();
		updateTexture(file);
	}

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
	_image_index = std::abs(index %  (int)getImageCount());
	const auto& frame = _sprite.frames().at(_image_index);
	updateVertices();
}
void GSprite::setImageIndex(int index, gm::DataWinFile& file) {
	setImageIndex(index);
	updateTexture(file);
}

