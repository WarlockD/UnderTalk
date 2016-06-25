#pragma once
#include "Global.h"

// simple class that contains a sprite frame

class GSprite :  public sf::Drawable {
	const std::string& _name;
	const std::vector<sf::Sprite>& _frames;
	int _frame;
public:
	//static GSprite LoadUndertaleSprite(int index);
//	static GSprite LoadUndertaleSprite(const std::string& name);
	void setImageIndex(int index) { _frame = index % _frames.size(); }
	int getImageIndex() const { return _frame; }
	const std::string& getName() { return _name;  }
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(_frames[_frame], states);
	}
};