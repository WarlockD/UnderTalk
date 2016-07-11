#include "room.h"
using namespace sf;

RoomObject::RoomObject(Room* room) : DrawableObject(), _movmentVector(0.0f, 0.0f), _gravityVector(0.0f, 0.0f), _gravity(0.0f), _direction(0.0f),  _room(room), _visiable(false) {
	setParent(room);
}

DrawableObject::~DrawableObject() {
	if (_parent != nullptr) {
		_parent->_objects.erase(this);
		_parent = nullptr;
		_root = nullptr;
	}
}
void DrawableObject::setParent(DrawableObject* parent) {
	if (_parent != nullptr) {
		_parent->_objects.erase(this);
		_root->_sortedObjects.erase(this);
		_parent = nullptr;
	}
	if (parent != nullptr) {
		_parent = parent;
		_root = getRoot();
		_parent->_objects.emplace(this);
		_root->_objectsChanged = true;
	}
	else if (this->_onheap) delete this;
}

void DrawableObject::addChild(DrawableObject* object,  int tag) {
	object->setParent(this);
	object->setTag(tag);
}

void DrawableObject::removeChild(DrawableObject* object) {
	object->setParent(nullptr);
}
void DrawableObject::removeChild(int tag) {
	for (auto it = _objects.begin(); it != _objects.end(); it++) {
		if ((*it)->getTag() == tag) (*it)->setParent(nullptr);
	}
}

void DrawableObject::addSelfandRecursiveOthers(t_sortedObjectList& list) {
	list.push_back(this);
	if (!_objects.empty()) { for (auto o : _objects) o->addSelfandRecursiveOthers(list); }
}
void DrawableObject::step(float dt) {
	if(!_objects.empty()) for (auto o : _objects) o->step(dt);
}
void DrawableObject::resortObjects() {
	if (_root == nullptr && _objectsChanged) {
		_sortedObjects.clear();
		addSelfandRecursiveOthers(_sortedObjects);
		_sortedObjects.sort();
		_objectsChanged = false;
	}
}

RoomObject* Room::instance_create(float x, float y, int index) {
	RoomObject* obj = new RoomObject(this); // auto adds to the child system
	obj->setPosition(x, y);
	obj->setTag(index); // index is the tag
	return obj;
}

TileMap::TileMap(const Undertale::Room& room) {
	loadRoom(room);

}
TileMap::TileMap(size_t index) {
	loadRoom(GetUndertale().LookupRoom(index));
}
void TileMap::loadRoom(size_t index) {
	loadRoom(GetUndertale().LookupRoom(index));
}
void TileMap::loadRoom(const Undertale::Room& room) {
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
			if (tile_map.getNativeHandle() == 0) {
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