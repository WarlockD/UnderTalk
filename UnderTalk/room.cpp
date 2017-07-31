#include "room.h"
#include "queue.h"

using namespace sf;

//https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
inline static uint32_t  hash32(uint32_t x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}
inline static uint64_t hash64(uint64_t x) {
	x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
	x = x ^ (x >> 31);
	return x;
}

Room::Room(gm::DataWinFile& file) : _file(file), _view(sf::FloatRect(0.0f, 0.0f, 640.0f, 480.0f)), _objects(64) {
}
Room::~Room() {
	for (auto& a : _objects) {
		RoomObject* o, *to;
		LIST_FOREACH_SAFE(o, &a, _index_hash, to) {
			if (o->_object_flags & DYNAMIC_FLAG)
				delete o;
		}
	}
}
void Room::unloadRoom() {
	for (auto& a : _objects) {
		RoomObject* o, *to;
		LIST_FOREACH_SAFE(o, &a, _index_hash, to) {
			LIST_REMOVE(o, _index_hash);
			if (o->_object_flags & DYNAMIC_FLAG)
				delete o;
		}
	}
}
void Room::loadRoom(uint32_t index) {
	if (index != _room.index()) {
		unloadRoom();
		_room = _file.resource_at<gm::Room>(index);
		if (_room.valid()) {
			_tiles.loadRoom(_file,_room);
		}
	}
}

RoomObject* Room::findObject(RoomObject* obj) {
	if (obj && obj->_room == this) return obj;
	return nullptr;
}
const RoomObject* Room::findObject(RoomObject* obj) const {
	if (obj && obj->_room == this) return obj;
	return nullptr;
}
const RoomObject* Room::findObject(uint32_t index) const {
	uint32_t hash = hash32(index);
	auto& start = _objects[hash % _objects.size()];
	const RoomObject* ret;
	LIST_FOREACH(ret, &start, _index_hash) {
		if (ret->getIndex() == index) return ret;
	}
	return nullptr;
}
RoomObject* Room::findObject(uint32_t index) {
	uint32_t hash = hash32(index);
	auto& start = _objects[hash % _objects.size()];
	RoomObject* ret;
	LIST_FOREACH(ret, &start, _index_hash) {
		if (ret->getIndex() == index) return ret;
	}
	return nullptr;
}
bool Room::deleteObject(RoomObject* object) {
	if (object && object->_room == this) {
		LIST_REMOVE(object, _index_hash);
		object->_room = nullptr;
		if (object->_object_flags & DYNAMIC_FLAG)
			_objects_to_delete.emplace_back(object);
		return true;
	}
	return false;
}
bool Room::removeObject(RoomObject& obj) {
	return deleteObject(&obj);
}
bool Room::removeObject(uint32_t index) {
	return deleteObject(findObject(index));
}
size_t Room::removeObjects(uint32_t index) {
	RoomObject* s = findObject(index);
	size_t count = 0;
	while (s) {
		RoomObject* next = LIST_NEXT(s, _index_hash);
		if (s->getIndex() == index) {
			deleteObject(s);
			count++;
		}
		s = next;
	}
	return count;
}
size_t Room::instanceCount(uint32_t index) const {
	const RoomObject* s = findObject(index);
	size_t count = 0;
	while (s && s->getIndex() == index) {
		count++;
		s = LIST_NEXT(s, _index_hash);
	}
	return count;
}
void Room::insertObject(RoomObject* obj) {
	if (obj && obj->_room == nullptr) {
		obj->_room = this;
		uint32_t hash = hash32(obj->getIndex());
		auto& start = _objects[hash % _objects.size()];
		RoomObject* s = LIST_FIRST(&start);
		if (s == nullptr || s->getIndex() >= obj->getIndex()) {
			LIST_INSERT_HEAD(&start, obj, _index_hash);
		}
		else {
			RoomObject* next;
			while(((next = LIST_NEXT(s, _index_hash)) != nullptr) && next->getIndex() < obj->getIndex()) s = next;
			LIST_INSERT_AFTER(s, obj, _index_hash);
		}
	}
}

void Room::step(float dt) {

	if (!LIST_EMPTY(&_sprite_list)) {
		RoomSprite* s;
		LIST_FOREACH(s, &_sprite_list, _sorted_list)
			s->step(dt);
	}
	if (!_objects_to_delete.empty()) {
		for (auto a : _objects_to_delete) delete a;
		_objects_to_delete.clear(); // delete all objects that need deleting after step
	}
}
void Room::draw(sf::RenderTarget& target, sf::RenderStates states) const  {
	target.setView(_view);
	target.draw(_tiles, states);
	if (!LIST_EMPTY(&_sprite_list)) {
		RoomSprite* s;
		LIST_FOREACH(s, &_sprite_list, _sorted_list)
			s->draw(target, states);
	}
}
// RoomSprite is inserted into the list, must be removed first
void RoomSprite::_insert_sort() {
	if (!_room) return;
	RoomSprite* s = LIST_FIRST(&_room->_sprite_list);
	if (s == nullptr || s->getDepth() >= this->getDepth()) {
		LIST_INSERT_HEAD(&_room->_sprite_list, this, _sorted_list);
	}
	else {
		RoomSprite* next;
		while(((next = LIST_NEXT(s, _sorted_list)) != nullptr) && next->getDepth() < this->getDepth()) s = next;
		LIST_INSERT_AFTER(s, this, _sorted_list);
	}
}

RoomSprite::RoomSprite(Room& room,float depth) : GSprite(), VisablitySettings(), MovementSettings(), _room(&room), _parentTrasform(nullptr) {
	setDepth(depth);
	_insert_sort();
}
RoomSprite::RoomSprite(Room& room, uint32_t sprite_index, float depth) :GSprite(room._file, sprite_index), _room(&room), VisablitySettings(), MovementSettings(), _parentTrasform(nullptr) {
	setDepth(depth);
	_insert_sort();
}
RoomSprite::~RoomSprite() {
	if (_room) {
		LIST_REMOVE(this, _sorted_list);
		_room = nullptr;
	}
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

void RoomSprite::setUndertaleSprite(uint32_t index) {
	assert(_room);
	GSprite::setUndertaleSprite(_room->_file, index);
}
void  RoomSprite::setFrame(const gm::SpriteFrame& frame) {
	GSpriteFrame::setFrame(_room->_file, frame);
}
void RoomSprite::setDepth(float depth)  {
	if (getDepth() != depth) {
		VisablitySettings::setDepth(depth);
		LIST_REMOVE(this, _sorted_list);
		_insert_sort();
	}
}

RoomSprite& RoomObject::create_sprite(int index) {
	_sprites.emplace_back(RoomSprite(*_room, (uint32_t)index));
	return _sprites.back();
}
void RoomObject::setObject(gm::Object obj) { 
	if (obj.index() != _object.index()) {
		_object = obj;
		setUndertaleSprite(_object.sprite_index());
		setDepth((float)_object.depth());
		_sprites.clear();
	}
}

RoomObject::RoomObject(Room& room, gm::Object object) : RoomSprite(room, object.sprite_index(), (float)object.depth()), _object(object) {
	_room->insertObject(this);
}

RoomObject::RoomObject(Room& room, uint32_t index) : RoomObject(room,room._file.resource_at<gm::Object>(index)) {}

RoomObject::~RoomObject() {
	if (_room) {
		assert(!(_object_flags & Room::DYNAMIC_FLAG));
		_room->deleteObject(this);
	}
}


void RoomObject::removeSelf() {
	if(_room) _room->deleteObject(this);
}

void RoomObject::removeSprite(uint32_t index) {
	_sprites.remove_if([index](RoomSprite& s) { return s.getIndex() == index; });
}
void RoomObject::removeSprite(RoomSprite& sprite) {
	_sprites.remove_if([sprite](RoomSprite& s) { return &s== &sprite; });
}



struct object_list_sort {
	bool operator()(const RoomSprite& l, const RoomSprite& r) const { return l.getDepth() < r.getDepth(); }
};
void RoomObject::step(float dt) {
	RoomSprite::step(dt);
}

void RoomObject::draw(sf::RenderTarget& target, sf::RenderStates states) const  {
	RoomSprite::draw(target, states);
	if (!_sprites.empty()) {
		states.transform *= getTransform(); // make sure the sprites are realitive to us
		for (const auto& s : _sprites)
			s.draw(target, states);
	}
}


void TileMap::unloadRoom() {
	_hasTiles = false;  // dosn't delete the resource though

}
void TileMap::loadRoom(gm::DataWinFile& file, const gm::Room& room) {
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
			//	auto& frame = room.backgrounds().at(tile.background_index);
				
				auto& frame = file.resource_at<gm::Background>(tile.background_index).frame();
		//		 GetUndertale().LookupBackground(tile.background_index).frame();
				IntRect rect(frame.x, frame.y, frame.width, frame.height);
				if (!tile_map.loadFromImage(Undertale::GetTextureImage(file,frame.texture_index), rect)) {
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