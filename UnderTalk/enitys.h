#pragma once
#include "Global.h"
#include "Global.h"

#include "node.h"
#include "gsprites.h"


// simple tilemap from examples
class TileMap : public sf::Drawable
{
	sf::RenderTexture _tiles;
	sf::Vertex _vertices[4]; // We draw to a render texture and this is the size of it
	Undertale::Room _room;
	bool _hasTiles;
public:
	TileMap() : _hasTiles(false) {}
	void loadRoom(const Undertale::Room& room);
	void unloadRoom();
	TileMap(size_t index);
	void loadRoom(size_t index);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


class SpriteComponent : public GSprite, public sf::Drawable {
public:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (valid()) {
			states.texture = getTexture().get();
			target.draw(getVertices(), getVerticesCount(), getVerticesType(), states);
		}
	}
};
using PositionComponent = sf::Transformable;

using ObjectComponent = Undertale::Room::Object;

class StaticComponent {
	sf::FloatRect _rect;
public:
	StaticComponent() : _rect() {}
	StaticComponent(const sf::FloatRect& box) : _rect(box) {}
	const sf::FloatRect getRect() const { return _rect; }
	void setRect(const sf::FloatRect& rect) { _rect = rect; }
};

using CompList = entityplus::component_list<PositionComponent,VelocityComponent, GravityComponent, SpriteComponent, StaticComponent, DepthComponent, ObjectComponent>;
struct PlayerTag {}; // tag


using TagList = entityplus::tag_list<PlayerTag>;
using ManagerType = entityplus::entity_manager<CompList, TagList>;
using entity_t = typename ManagerType::entity_t;



#if 0
Position(x, y)
Velocity(x, y)
Physics(body)
Sprite(images, animations)
Health(value)
Character(name, level)
Player(empty)
#endif