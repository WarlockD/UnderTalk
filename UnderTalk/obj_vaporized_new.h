#pragma once
#include "Global.h"
#include "gsprites.h"



class obj_vaporized_new : public sf::Drawable, public sf::Transformable {
	std::vector<GSprite> _sprites; // slow but eh
public:
	obj_vaporized_new(const std::string& str);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
