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




GSpriteFrame::GSpriteFrame(const Undertale::SpriteFrame* frame, sf::Color color) {
	setFrame(frame);
	setColor(color);
}
template<typename Tval>
struct MyTemplatePointerHash1 {
	size_t operator()(const Tval* val) const {
		static const size_t shift = (size_t)log2(1 + sizeof(Tval));
		return (size_t)(val) >> shift;
	}
};
std::unordered_map <size_t, std::weak_ptr<GSpriteFrame::SpriteFrameCache>> GSpriteFrame::spriteFrameCache;

void GSpriteFrame::setFrame(const Undertale::SpriteFrame* frame) {
	//MakeSpriteTriangles(_vertices, textRect, Vector2f(frame.offset_x, frame.offset_y));
	//_texture = Undertale::GetTexture(frame.texture_index);
	size_t index = (size_t)frame;

	auto it = spriteFrameCache.find(index);
	if (it != spriteFrameCache.end() && !it->second.expired()) {
		_frame = it->second.lock();
	}
	else {
		SpriteFrameCache* cframe = new SpriteFrameCache;
		cframe->rect = IntRect(frame->x, frame->y, frame->width, frame->height);
		auto image = Undertale::GetTextureImage(frame->texture_index);
		if (!cframe->texture.loadFromImage(*image)) {
			// couldn't load.  might be because the texture is to big.  If thats the case we got to cut it up
			if (cframe->texture.loadFromImage(*image, cframe->rect)) {
				cframe->rect.top = 0;
				cframe->rect.left = 0;
			}
			else { // total fail
				printf("Could not cut up texture\r\n");
				throw std::exception("Ugh");
			}
		}
		_frame = std::shared_ptr<SpriteFrameCache>(cframe);
		std::weak_ptr<SpriteFrameCache> wptr = _frame;
		spriteFrameCache.emplace(index, wptr);
	}
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


const sf::IntRect& GSpriteFrame::getTextureRect() const {
	return _frame->rect;
}
const sf::Texture& GSpriteFrame::getTexture() const {
	return _frame->texture;
}
void GSpriteFrame::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_frame) {
		states.texture = &_frame->texture;
		target.draw(_vertices, 6, sf::PrimitiveType::Triangles, states);
	}
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
	_frame.setFrame(sprite->frames()->at(_image_index = 0));
}

