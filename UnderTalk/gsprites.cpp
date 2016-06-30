#include "gsprites.h"
using namespace sf;

namespace {
	struct SpriteFrame {
		std::string& name;
		std::vector<sf::Sprite> frames;
		sf::IntRect bounds;
		sf::Vector2i size;

		std::vector<unsigned char> _mask;
	};
}
GSpriteFrame::GSpriteFrame(const Undertale::SpriteFrame& frame, sf::Color color) {
	setFrame(frame);
	setColor(color);
}

void GSpriteFrame::setFrame(const Undertale::SpriteFrame& frame) {
	MakeSpriteTriangles(_vertices, IntRect(frame.x, frame.y, frame.width, frame.height), Vector2f(frame.offset_x, frame.offset_y));
	_texture = Undertale::GetCachedTexture(frame.texture_index);
}
void GSpriteFrame::setColor(const sf::Color& color) {
	_vertices[0].color = color;
	_vertices[1].color = color;
	_vertices[2].color = color;
	_vertices[3].color = color;
	_vertices[4].color = color;
	_vertices[5].color = color;
}
void GSpriteFrame::insertIntoVertexList(sf::VertexArray& list) const {
	assert(list.getPrimitiveType() == sf::PrimitiveType::Triangles);
	for (int i = 0; i < 6; i++) list.append(_vertices[i]);
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
	const size_t frameCount = sprite->frames()->size();
	_vertices.resize(frameCount * 6);
	_texture = Undertale::GetCachedTexture(sprite->frames()->at(0)->texture_index);
	for (uint32_t i = 0; i < frameCount; i++) {
		const auto frame = sprite->frames()->at(i); 
		MakeSpriteTriangles(_vertices.data() + (i * 6), IntRect(frame->x, frame->y, frame->width, frame->height), _color, Vector2f(frame->offset_x, frame->offset_y));
#ifdef _DEBUG
		// I havn't ran into a sprite that is NOT on the same texture as the other frames.
		// but just in case lets check it
		const Texture* dtexture = Undertale::GetCachedTexture(sprite->frames()->at(i)->texture_index);
		assert(dtexture == _texture);
#endif
	}
}
void GSprite::insertIntoVertexList(sf::VertexArray& list) const {
	assert(list.getPrimitiveType() == sf::PrimitiveType::Triangles);
	for (int i = 0; i < 6; i++) list.append(_vertices[i]);
}
void GSprite::setColor(const sf::Color& color) {
	for (auto& v : _vertices) v.color = color;
	_color = color;
}

static GSprite LoadUndertaleSprite(int index) {

}
/*

GSprite GSprite::LoadUndertaleSprite(int index) {
	const sf::Texture& GetTexture(int index);
	return GSprite();
}
GSprite GSprite::LoadUndertaleSprite(const std::string& name) {
	return GSprite();
}
*/