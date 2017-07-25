#include "room.h"
using namespace sf;




entity_t instance_create(float x, float y, int index) {

}

void Room::loadRoom(int index) {
	_room = GetUndertale().LookupRoom(index);
	if (_room.valid()) {
		_tiles.loadRoom(_room);
	}
}
void Room::step(float dt)  {
#if 0
	_manager.for_each<PositionComponent,VelocityComponent>([](auto ent, auto &id) {
		
		std::cout << id.name_ << "\n";
	});


	bodyStep(dt);
	_current_frame += _image_speed;
	if ((std::abs(_current_frame) + 0.01f) >= 1.0f) { // we add some flub there
		int next_frame = (int)std::modf(_current_frame + 0.01f, &_current_frame); // this should work
		_sprite.setImageIndex(next_frame);
	}

	Node::step(dt);
#endif
}
void RoomObject::draw(sf::RenderTarget& target, sf::RenderStates states) const  {
	if (_sprite.getUndertaleSprite().valid()) {
		// we use OUR transform
		states.transform *= getTransform();
		states.texture = _sprite.getTexture().get();
		target.draw(_sprite.getVertices(), _sprite.getVerticesCount(), _sprite.getVerticesType(), states);
	}
}


TileMap::TileMap(size_t index) {
	loadRoom(GetUndertale().LookupRoom(index));
}
void TileMap::loadRoom(size_t index) {
	loadRoom(GetUndertale().LookupRoom(index));
}
void TileMap::unloadRoom() {
	_hasTiles = false;  // dosn't delete the resource though
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