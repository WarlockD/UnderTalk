#include "UndertaleLib.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <sstream>

static uint32_t constexpr ChunkToInt(const char* name) {
	return name[3] << 24 | name[2] << 16 | name[1] << 8 | name[0];
}
//	static const int test = ChunkToInt("TXTR");
enum class ChunkType : unsigned int {
	TXTR=0,
	BGND,
	TPAG,
	SPRT,
	ROOM,
	AUDO,
	SOND,
	FONT,
	OBJT,
	PATH,
	SCPT,
	CODE,
	VARS,
	FUNC,
	_CMAX

};

namespace Undertale {

	/* std::unordered_map<int, std::unique_ptr<Sprite>> _sprites;
public:
	enum class ChunkType : unsigned int {
		TXTR,
		BGND,
		TPAG,
		SPRT,
		ROOM,
		AUDO,
		SOND,
		FONT,
		OBJT,
		PATH,
		SCPT,
		CODE,
		VARS,
		FUNC,
		GEN8,
		_CMAX

	};
	struct Chunk {
		char name[4];
		int size;
		int count;
		int offsets[];  // used for fast lookups
	};
	*/
	UndertaleFile::UndertaleFile() {

	}
	void UndertaleFile::internalReset() {
		// we don't clear data as movement or file opening will do that
		_chunks.clear(); // chunks are cleared
		_lookup.clear();
		_spriteCache.clear(); // so are sprites, eveything is invalidated
	}
	bool UndertaleFile::internalParse() {
		// must be reset before we get here
		const Chunk* c_form = reinterpret_cast<const Chunk*>(_data.data());
		uint32_t max_size = (uint32_t)c_form->size;
		uint32_t pos = 8;
		std::vector<const Chunk*> chunks;
		while (pos < max_size) {
			const Chunk* chunk = reinterpret_cast<const Chunk*>(_data.data()+pos);
			chunks.push_back(chunk);
			pos += chunk->size + 8;
		}
		_chunks.resize(chunks.size(),nullptr); // make sure we have nulls on the ones we don't need
		for (const Chunk* c : chunks) {
			switch (c->iname) {// I LOVE constant expressions
			case ChunkToInt("SPRT"): _chunks[(uint32_t)ChunkType::SPRT] = c; break;
			case ChunkToInt("BGND"): _chunks[(uint32_t)ChunkType::BGND] = c; break;
			case ChunkToInt("TPAG"): _chunks[(uint32_t)ChunkType::TPAG] = c; break;
			case ChunkToInt("ROOM"): _chunks[(uint32_t)ChunkType::ROOM] = c; break;
			case ChunkToInt("AUDO"): _chunks[(uint32_t)ChunkType::AUDO] = c; break;
			case ChunkToInt("SOND"): _chunks[(uint32_t)ChunkType::SOND] = c; break;
			case ChunkToInt("FONT"): _chunks[(uint32_t)ChunkType::FONT] = c; break;
			case ChunkToInt("OBJT"): _chunks[(uint32_t)ChunkType::OBJT] = c; break;
			case ChunkToInt("PATH"): _chunks[(uint32_t)ChunkType::PATH] = c; break;
			case ChunkToInt("GEN8"): _chunks[(uint32_t)ChunkType::GEN8] = c; break;
			case ChunkToInt("CODE"): _chunks[(uint32_t)ChunkType::CODE] = c; break;
			case ChunkToInt("VARS"): _chunks[(uint32_t)ChunkType::VARS] = c; break;
			case ChunkToInt("FUNC"): _chunks[(uint32_t)ChunkType::FUNC] = c; break;
			}
				
			// try to look up and match
		}
//		assert(false);
		return true;
	}
	bool UndertaleFile::loadFromFilename(const std::string& filename) {
		internalReset();
		std::fstream file(filename, std::ios::in | std::ios::binary);
		if (!file) {
			std::cerr << "File '" << filename.c_str() << "' could not be opened.  Code: " << std::strerror(errno) << std::endl;
			return false;
		}

		unsigned int size = 0;
		file.seekg(0, std::ios::end);
		_data.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read((char*)_data.data(), _data.size());
		file.close();
		return internalParse();
	}
	const Sprite& UndertaleFile::LookupSprite(int index) const {
		auto it = _spriteCache.find(index);
		if (it == _spriteCache.end()) return Sprite();
		else return *it->second;
	}
	bool UndertaleFile::loadFromData(const std::vector<uint8_t>& data) {
		internalReset();
		_data = data; // make a copy here because we MUST have a local copy
		return internalParse();
	}// w want it, so its a move
	//bool UndertaleFile::loadFromData(const std::vector<uint8_t>& _data) {
	template<typename T> void debug_int(const T* ptr, int count) {
		std::vector<int> a;
		for (int i = 0; i < count; i++) {
			int value = *(reinterpret_cast<const int*>(ptr+i));
			a.push_back(value);
			printf("%i : %i  %x\n", i, value, value);
		}
	}
	std::string UndertaleFile::OffsetList::toString() const {

		std::stringstream ss;
		ss << "{ count :";
		ss << count;
		ss << ", offsets : [";
		for (int i = 0; i < count; i++) {
			if (i != 0) ss << ",";
			ss << std::hex << offsets[i];
		}
		ss << "] }";
		return ss.str();
	}
	//}// copy it instead
	const Sprite& UndertaleFile::LookupSprite(int index)  {
		if (index < 0) return Sprite();
		// ok looking up a sprite by index is easy first check the cache
		auto it = _spriteCache.find(index);
		if (it == _spriteCache.end()) { // if its not there, we got to look it up
			auto spriteChunk = getChunk(ChunkType::SPRT);
			if (index >= spriteChunk->count) return Sprite();
			uint32_t offset = spriteChunk->offsets[index];
			Sprite* sprite = new Sprite;
			sprite->_raw = reinterpret_cast<const Sprite::RawSprite*>(_data.data() + offset);
			sprite->_name = reinterpret_cast<const char*>(_data.data() + sprite->_raw->name_offset);
			sprite->_index = index;
			uint8_t* frame_offset = _data.data() + offset + +sizeof(Sprite::RawSprite);

			const OffsetList* frames = reinterpret_cast<const OffsetList*>(frame_offset);
			for (uint32_t i = 0; i < frames->count; i++) {
				sprite->_frames.push_back(reinterpret_cast<const SpriteFrame*>(_data.data() + frames->offsets[i]));
			}
	
			// makss
			uint8_t* mask_offset = frame_offset + (frames->count * sizeof(uint32_t))+4;
	
			int mask_count = *mask_offset;
			if (mask_count > 0) {
				mask_offset += sizeof(uint32_t);
				int stride = (sprite->width() + 7) / 8;
				for (int i = 0; i < mask_count; i++) {
					BitMask mask;
					mask._width = sprite->width();
					mask._height = sprite->height();
					mask._raw = mask_offset;
					mask_offset += stride * sprite->height();
					sprite->_masks.emplace_back(mask);
				}
			}
			_spriteCache.emplace(std::make_pair((uint32_t)index,sprite));
			const IName* cast = dynamic_cast<const IName*>(sprite);
			_lookup.emplace(std::make_pair(std::string(sprite->name()), cast));
			return *sprite;
		}
		return *it->second;
	}
	const Sprite& UndertaleFile::LookupSprite(const std::string& name) const {
		return Sprite();
	}
}


//std::vector<uint8_t*> _data;
//std::unordered_map<std::string, const IName*> _lookup;
