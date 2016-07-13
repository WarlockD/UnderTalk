#include "UndertaleLib.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <sstream>
#include <type_traits>
#include <algorithm>
static uint32_t constexpr ChunkToInt(const char* name) {
		return  name[3] << 24 | name[2] << 16 | name[1] << 8 | name[0];
}

static inline void ChunkToChar(const uint8_t*ptr, char chars[5]) {
	chars[0] = ptr[0]; chars[1] = ptr[1]; chars[2] = ptr[2]; chars[3] = ptr[3]; chars[4] = 0;
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
	STRG,
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
		_nameCache.clear();
	}
	String UndertaleFile::getUndertaleString(int offset) const {
		const uint8_t* ptr = _data.data() + offset - sizeof(uint32_t);
		const UndertaleString* ustr = (const UndertaleString*)(ptr);
		return String(ustr->u_str, ustr->length);
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
			case ChunkToInt("TXTR"): _chunks[(uint32_t)ChunkType::TXTR] = c; break;
			case ChunkToInt("STRG"): _chunks[(uint32_t)ChunkType::STRG] = c; break;
			}
				
			// try to look up and match
		}
//		assert(false);
		// ok we load all strings
		auto strChunk = getChunk(ChunkType::STRG);
		_strings.reserve(strChunk->count);
		for (size_t i = 0; i < strChunk->count; i++) {
			const uint8_t* ptr = _data.data() + strChunk->offsets[i];
#ifdef _DEBUG
			uint32_t len = *((const uint32_t*)ptr);
			const char* str = (const char*)(ptr + sizeof(uint32_t));
#endif
			auto result = _strings.emplace((const UndertaleString*)(ptr));
			assert(result.second);
		}

		return true;
	}
	bool UndertaleFile::loadFromFilename(const std::string& filename) {
		internalReset();
		std::fstream file(filename, std::ios::in | std::ios::binary);
		if (!file || file.bad()) {
			std::cerr << "File '" << filename.c_str() << "' could not be opened.  Code: " << std::strerror(errno) << std::endl;
			return false;
		}

		unsigned int size = 0;
		file.seekg(0, std::ios::end);
		_data.resize((size_t)file.tellg());
		file.seekg(0, std::ios::beg);
		file.read((char*)_data.data(), _data.size());
		file.close();
		return internalParse();
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

	template<class C> void UndertaleFile::fillList(size_t offset, std::vector<const C*>& list) const {
		const OffsetList* olist = reinterpret_cast<const OffsetList*>(_data.data() + offset);
		if (olist->count > 0) {
			for (int i = 0; i < olist->count; i++)
				list.push_back(reinterpret_cast<const C*>(_data.data() + olist->offsets[i]));
		}
		else list.clear();
	}
	template<class C, class P> void UndertaleFile::fillList(size_t offset, std::vector<const C*>& list, P pred) const {
		const OffsetList* olist = reinterpret_cast<const OffsetList*>(_data.data() + offset);
		if (olist->count > 0) {
			for (int i = 0; i < olist->count; i++) {
				const C* obj = reinterpret_cast<const C*>(_data.data() + olist->offsets[i]);
				if (pred(*obj)) list.push_back(obj);
			}
		}
		else list.clear();
	}	
	template<class T> const uint8_t* UndertaleFile::preCreateResorce(int index, T&res) const {
		const Chunk* chunk = getChunk(T::ResType);
		if (index >= 0 || (uint32_t)index < chunk->size) {
			const uint8_t* ptr = _data.data() + chunk->offsets[index];
			res._raw = reinterpret_cast<const T::RawResourceType*>(ptr);
			res._name = getUndertaleString(res._raw->name_offset);//    reinterpret_cast<const char*>(_data.data() + font->_raw->name_offset);
			res._index = index;
			return ptr + sizeof(T::RawResourceType);
		}
		else return nullptr;
	}
	template<> Font UndertaleFile::createResource(int index) const {
		const Chunk* chunk = getChunk(Font::ResType);
		Font font;
		const uint8_t* ptr = preCreateResorce(index, font);
		if (ptr != nullptr) {
			font._description = getUndertaleString(font._raw->description_offset); //reinterpret_cast<const char*>(_data.data() + font->_raw->description_offset);
			font._frame = reinterpret_cast<const SpriteFrame*>(_data.data() + font._raw->frame_offset);
			font._glyphs = OffsetVector<Font::Glyph>(_data.data(), ptr);
		}		
		return font;
	}
	template<> Background UndertaleFile::createResource(int index) const {
		const Chunk* chunk = getChunk(Background::ResType);
		Background background;
		const uint8_t* ptr = preCreateResorce(index, background);
		if (ptr != nullptr) {
			background._frame = reinterpret_cast<const SpriteFrame*>(_data.data() + background._raw->frame_offset);
		}
		return background;
	}

	template<> Room UndertaleFile::createResource(int index) const {
		const Chunk* chunk = getChunk(Room::ResType);
		Room room;
		const uint8_t* ptr = preCreateResorce(index, room);
		if (ptr != nullptr) {
			room._caption = getUndertaleString(room._raw->caption_offset);
			room._tiles = OffsetVector<Room::Tile>(_data.data(), room._raw->tiles_offset);
			room._backgrounds = OffsetVector<Room::Background>(_data.data(), room._raw->background_offset);
			room._objects = OffsetVector<Room::Object>(_data.data(), room._raw->object_offset);
			room._views = OffsetVector<Room::View>(_data.data(), room._raw->view_offset);
		}
		return room;
	}


	template<> Sprite UndertaleFile::createResource(int index) const {
		const Chunk* chunk = getChunk(Sprite::ResType);
		Sprite sprite;
		const uint8_t* ptr = preCreateResorce(index, sprite);
		if (ptr != nullptr) {
			sprite._frames = OffsetVector<SpriteFrame>(_data.data(), ptr);
			// makss
			// side note, we have to subtract sizeof(uint32_t) because of the _raw->frame_offsets[1].  Had to put a 1 in there to make the compiler happy
			const uint8_t* mask_offset = ptr + sizeof(uint32_t) + (sizeof(uint32_t) * sprite.frames().size());

			int mask_count = *((const int*)mask_offset);
			if (mask_count > 0) {
				mask_offset += sizeof(uint32_t);
				int stride = (sprite.width() + 7) / 8;
				for (int i = 0; i < mask_count; i++) {
					BitMask mask;
					mask._width = sprite.width();
					mask._height = sprite.height();
					mask._raw = mask_offset;
					mask_offset += stride * sprite.height();
					sprite._masks.emplace_back(mask);
				}
			}
		}
		return sprite;
	}



	// I could read a bitmap here like I did in my other library however
	// monogame dosn't use Bitmaps, neither does unity, so best just to make a sub stream
	//static const char* pngSigStr = "\x89PNG\r\n\x1a\n";

	static const uint8_t pngSig[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
	Texture UndertaleFile::LookupTexture(int index) const {
		auto txtChunk = getChunk(ChunkType::TXTR);
		if (index >= 0 && (uint32_t)index < txtChunk->count)
		{
			uint32_t offset = *(const uint32_t*)(_data.data() + txtChunk->offsets[index] + sizeof(int));
			const uint8_t* ptr = _data.data() + offset;
			const uint8_t* start = ptr;
			assert(memcmp(ptr, pngSig, 8) == 0);
			ptr += 8;
			// caculate png size of file
			
			
			bool done = false;
			while (!done) {
				int len = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]); ptr += sizeof(uint32_t);
				uint32_t chunk = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]); 
				char debug[5]; //memcpy(debug, ptr, 4); debug[4] = 0;
				ChunkToChar(ptr, debug);
				ptr += sizeof(uint32_t);
				assert(len >= 0);
				if(memcmp(debug,"IEND",4) ==0) 
					done = true;
				ptr += len + sizeof(uint32_t); // + the crc
			}
			size_t pngSize = ptr - start;
			return Texture(start, pngSize);
		}
			
		else
			return Texture();
	}
	std::vector<Font> UndertaleFile::ReadAllfonts() {
		std::vector<Font> fonts;
		const Chunk* chunk = getChunk(Font::ResType);
		fonts.reserve(chunk->count);
		for (uint32_t i = 0; i < chunk->count; i++) fonts.emplace_back(createResource<Font>(i));
		
		return fonts;
	}
	Sprite UndertaleFile::LookupSprite(int index) {return createResource<Sprite>(index); }
	Room UndertaleFile::LookupRoom(int index) { return createResource<Room>(index); }

	Background UndertaleFile::LookupBackground(int index) { return createResource<Background>(index); }
}


//std::vector<uint8_t*> _data;
//std::unordered_map<std::string, const IName*> _lookup;
