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

/*

GSprite GSprite::LoadUndertaleSprite(int index) {
	const sf::Texture& GetTexture(int index);
	return GSprite();
}
GSprite GSprite::LoadUndertaleSprite(const std::string& name) {
	return GSprite();
}
*/