#include "room.h"
using namespace sf;




entity_t instance_create(float x, float y, int index) {

}

void Room::loadRoom(uint32_t index) {
	_room = _file.resource_at < gm::Room>(index);
	if (_room.valid()) {
		_tiles.loadRoom(_room);
	}
}
void Room::deleteObject(RoomObject* object) {

}
bool Room::removeObject(RoomObject& obj) {
	if (obj._room != nullptr) { 
		auto& owned = _objects.find(&obj);
		if (owned != _objects.end()) {
			_objects_to_delete.emplace_back(std::move(owned));
			_objects.erase(owned);
			obj._room = nullptr;
			return true;
		}
	}
	return false;
}

size_t Room::removeObject(uint32_t index) {
	auto& range = _objects.equal_range(index);
	if (range.first != _objects.end()) {
		size_t count = std::distance(range.first, range.second);
		if (count == 1) {
			removeObject(*const_cast<RoomObject*>(range.first->object()));
		} else {
			for (auto it = range.first; it != range.second;) {
				auto next = it;
				removeObject(*const_cast<RoomObject*>(it->object()));
				next++;
			}
			_object_list.clear(); // just clear it all as it might be a bunch of junk
			_objects_to_delete.insert(_objects_to_delete.end(), std::make_move_iterator(range.first), std::make_move_iterator(range.second));
		}
	}
	return 0;
}


RoomSprite& RoomObject::create_sprite(int index) {
	_sprites.emplace_back(_room->_file, index);
	return _sprites.back();
}

RoomObject::RoomObject(Room& room, gm::Object object) :_room(&room), _object(object) {
	_room->_objects.emplace(this);
}

RoomObject::RoomObject(Room& room, uint32_t index) :_room(&room), _object(room._file.resource_at<gm::Object>(index)) {
	_room->_objects.emplace(this);
}

RoomObject::~RoomObject() {
	if (_room) {
		_room->_objects.erase(this);
		_room = nullptr;
	}
}

void RoomObject::setDepth(float depth) { 
	if (getDepth() != depth) {
		_room->_object_list.clear();
		VisablitySettings::setDepth(depth);
	}
}
void RoomObject::removeSelf() {
	_room->removeObject(*this);
}
void RoomObject::setUndertaleSprite(uint32_t index) {
	RoomSprite::setUndertaleSprite(_room->_file, index);
}
void RoomObject::removeSprite(uint32_t index) {
	_sprites.remove_if([index](RoomSprite& s) { return s.getIndex() == index; });
}
void RoomObject::removeSprite(RoomSprite& sprite) {
	_sprites.remove_if([sprite](RoomSprite& s) { return &s== &sprite; });
}


void RoomSprite::step(float dt) {
	MovementSettings::step(dt);
	if (_frameSpeed != 0.0f) {
		_currentFrameTime += dt;
		if (_currentFrameTime >= std::abs(_frameSpeed)) {
			if (_frameSpeed > 0.0f)
				nextFrame();
			else
				prevFrame();
		}
	}
}
struct object_list_sort {
	bool operator()(const RoomSprite& l, const RoomSprite& r) const { return l.getDepth() < r.getDepth(); }
};
void RoomObject::step(float dt) {
	RoomSprite::step(dt);
	if (!_sprites.empty()) {
		_sprites.sort(object_list_sort{});
		for (auto& s : _sprites)
			s.step(dt);
	}
}

void RoomObject::draw(sf::RenderTarget& target, sf::RenderStates states) const  {
	RoomSprite::draw(target, states);
	if (!_sprites.empty()) {
		states.transform *= getTransform(); // make sure the sprites are realitive to us
		for (const auto& s : _sprites)
			s.draw(target, states);
	}
}
void Room::step(float dt) {
	if (!_objects.empty()) {
		bool resort = _object_list.empty();
		for (auto& a : _objects) {
			RoomObject& obj = *a.object();
			if (resort) _object_list.emplace(obj);
			obj.step(dt);
		}
	}
	if (!_objects_to_delete.empty()) 
		_objects_to_delete.clear(); // delete all objects that need deleting after step
}

void TileMap::unloadRoom() {
	_hasTiles = false;  // dosn't delete the resource though
}
void TileMap::loadRoom(const gm::Room& room) {
	if (room.tiles().size() > 0) {
		_hasTiles = true;
		if (!_tiles.create(room.width(), room.height(), true)) {
			printf("Could not create rendertexture for tiles");
			exit(1);
		}
		_tiles.clear(Color(0,0,0,0));
		{ // set up the verts
			float width = static_cast<float>(room.width());
			float height = static_cast<float>(room.height());
			_vertices[0] = Vertex(Vector2f(0.0f, 0.0f), Color::White, Vector2f(0.0f, 0.0f));
			_vertices[1] = Vertex(Vector2f(0.0f, height), Color::White, Vector2f(0.0f, height));
			_vertices[2] = Vertex(Vector2f(width, 0.0f), Color::White, Vector2f(width, 0.0f));
			_vertices[3] = Vertex(Vector2f(width, height), Color::White, Vector2f(width, height));
		}
		std::unordered_map<size_t, sf::Texture> textures;
		uint32_t texture_index = -1;
		Sprite drawing_tile;
		for (auto& tile : room.tiles()) {
			auto& tile_map = textures[tile.background_index];
			if (tile_map.getNativeHandle() == 0) { // cache the textures if we have several of them
				auto& frame = GetUndertale().LookupBackground(tile.background_index).frame();
				IntRect rect(frame.x, frame.y, frame.width, frame.height);
				if (!tile_map.loadFromImage(Undertale::GetTextureImage(frame.texture_index), rect)) {
					printf("Cannot load texture");
					exit(1);
				}
			}
			drawing_tile.setTexture(tile_map);
			drawing_tile.setTextureRect(IntRect(tile.offset_x, tile.offset_y, tile.width, tile.height));
			drawing_tile.setPosition((float)tile.x, (float)tile.y);
			drawing_tile.setScale(tile.scale_x, tile.scale_y);
			_tiles.draw(drawing_tile);
		}
		_tiles.display();
	}
	else _hasTiles = false;
	
}


void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (_hasTiles) {
		states.texture = &_tiles.getTexture();
		target.draw(_vertices,4,PrimitiveType::TriangleStrip, states); 
	}
}