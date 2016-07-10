#include "room.h"

RoomObject::RoomObject(Room* room) : _movmentVector(0.0f, 0.0f), _gravityVector(0.0f, 0.0f), _gravity(0.0f), _direction(0.0f),  _room(room), _visiable(false) {
	if (_room != nullptr) _room->addChild(this);
}

RoomObject::~RoomObject() {
	if (_room != nullptr) {
		_room->_objects.erase(this);
		_room = nullptr;
	}
}


void Room::addChild(RoomObject* object) { _objects.emplace(object); _objectsChanged = true;  }
void Room::removeChild(RoomObject* object) { 
	object->_room = nullptr;  // important or ~RoomObject will do it for us!
	_objects.erase(object); 
	_sortedObjects.erase(object);  // don't need to resort as the object was just removed?
	delete object; 
}
void Room::removeChild(int tag) {
	for (auto it = _objects.begin(); it != _objects.end(); it++) {
		if ((*it)->getTag() == tag) it = _objects.erase(it);
	}
}
void Room::resortObjects() {
	_sortedObjects = std::move(t_sortedObjectList(_objects.begin(), _objects.end()));
}

TileMap::TileMap(Undertale::UndertaleFile& res, const Undertale::Room* room) {
	loadRoom(res, room);

}

sf::Texture CreateCustomTexture(Undertale::UndertaleFile& res, const Undertale::Room* room, std::unordered_map<size_t, sf::IntRect>& rects) {
	sf::Vector2u size;
	std::unordered_map<size_t, const Undertale::SpriteFrame*> frames;
	for (auto& tile : room->tiles()) {
		auto frame = frames[tile->background_index];
		if (frame == nullptr) {
			frame = res.LookupBackground(tile->background_index)->frame();
			size.x += frame->width;
			if (size.y < frame->height) size.y = frame->height;
				frames[tile->background_index] = frame;
		}
	}
	sf::Texture texture;
	texture.create(size.x, size.y);
	sf::Vector2u pos;
	for (auto it   : frames) {
		auto frame = it.second;
		sf::IntRect rect(frame->x, frame->y, frame->width, frame->height);
		sf::Image copy;
		copy.create(frame->width, frame->height);
		copy.copy(Undertale::GetTextureImage(frame->texture_index), 0, 0, rect);
		texture.update(copy, pos.x, pos.y);
		rect.left = pos.x;
		rect.top = pos.y;
		rects[it.first] = rect;
		pos.x += frame->width;
	}
	return std::move(texture);
}
void TileMap::loadRoom(Undertale::UndertaleFile& res, const Undertale::Room* room) {
	std::unordered_map<size_t, sf::IntRect> rects;
	_texture = CreateCustomTexture(res, room, rects);
	_vertices.clear();
	_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	// since the tiles could have diffrent backgrounds and while "I think" the backgrounds iwll
	// all be in the same texture, I cannot be 100% sure?  or I could just hack it sigh
	for (auto& tile : room->tiles()) {
		sf::IntRect rect = rects[tile->background_index];
		rect.left += tile->offset_x;
		rect.top += tile->offset_y;
		rect.width = tile->width;
		rect.height = tile->height;
		AppendSpriteTriangles(_vertices, rect, sf::Vector2f(tile->x, tile->y), sf::Vector2f(tile->scale_x, tile->scale_y));
	}
}