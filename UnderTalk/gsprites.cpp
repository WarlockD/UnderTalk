#include "gsprites.h"
using namespace sf;




//GSpriteFrame* cframe = new GSpriteFrame;
//cframe->_texRect = IntRect(frame->x, frame->y, frame->width, frame->height);


GSpriteFrame::GSpriteFrame(const Undertale::SpriteFrame* frame) {
	setFrame(frame);
}


void GSpriteFrame::setFrame(const Undertale::SpriteFrame* frame) {
	_texture_index = frame->texture_index;
	_frame = frame;
	_texRect = IntRect(frame->x, frame->y, frame->width, frame->height);
	_origin = Vector2u(frame->offset_x, frame->offset_y);
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


void GSprite::setUndertaleSprite(int index) {
	const Undertale::Sprite* sprite = GetUndertale().LookupSprite(index);
	setUndertaleSprite(sprite);
}
void GSprite::setUndertaleSprite(const std::string& name) {
	assert(false);
}
void GSprite::setUndertaleSprite(const Undertale::Sprite* sprite) {
	assert(sprite != nullptr);
	_sprite = sprite;
	GSpriteFrame gframe(sprite->frames()->at(0));
	_texture = gframe.getTexture(); // save a texture ref
	setTexture(*_texture.getTexture());
	setImageIndex(0);
}

