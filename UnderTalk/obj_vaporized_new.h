#pragma once
#include "Global.h"
#include "gsprites.h"



class obj_vaporized_new : public sf::Drawable, public sf::Transformable {
	sf::VertexArray _vertexes;
	std::vector <std::vector<TimeTransformableVertexArray>> _particles;
public:
	// spec true makes evey pixel independent of one another
	//
	obj_vaporized_new(uint32_t index,bool spec);
	obj_vaporized_new(const GSprite& sprite, bool spec);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
