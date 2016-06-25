#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef UNDERLIBRARY_DLL
#ifdef UNDERLIBRARY_EXPORTS
#define UNDERLIBRARY_API __declspec(dllexport) 
#else
#define UNDERLIBRARY_API __declspec(dllimport) 
#endif
#else
#define UNDERLIBRARY_API
#endif

// We load up the entire datawin file once the main object is created
// and its used with pointers to all the data
// because of this, this library really dosn't work in Big eadin systems.
namespace Undertale {
	// This structure is pointed to somewhere in the data win
	struct SpriteFrame {
		short x;
		short y;
		short width;
		short height;
		short offset_x;
		short offset_y;
		short crop_width;
		short crop_height;
		short original_width;
		short original_height;
		short texture_index;
	};
	struct IName {
		virtual const char*  name() const = 0;
		virtual ~IName() {}
	};
	class BitMask {
		int _width;
		int _height;
		const uint8_t* _raw;
		BitMask() : _width(0), _height(0), _raw(nullptr) {}
	public:
		int stride() const { return (_width + 7) / 8; }
		int width() const { return _width; }
		int height() const { return _height; }
		const uint8_t* raw() { return _raw; }
		const uint8_t* scaneline(int line) const { return _raw + (stride()*line); }
		// mainly here for a helper function, tells if a bit is set
		// its really here to describe how bitmaks are made
		bool isSet(int x, int y) const {
			uint8_t pixel = _raw[y * _width + x / 8];
			uint8_t bit = (7 - (x & 7) & 31);
			return ((pixel >> bit) & 1) != 0;
		}
		friend class UndertaleFile;
	};
	class Sprite : public IName {
		#pragma pack(1)
		struct RawSprite {
			int name_offset;
			int width;
			int height;
			int left;
			int right;
			int bottom;
			int top;
			int trasparent;
			int smooth;
			int preload;
			int mode;
			int colcheck;
			int original_x;
			int original_y;
			// int frame_count, int[] offsets
			// int mask_offsets int[] offsets
		};
		#pragma pack()
		int _index;
		const char* _name;
		const RawSprite* _raw;
		std::vector<const SpriteFrame*> _frames;
		std::vector<BitMask> _masks;
		friend class UndertaleFile;
		Sprite() : _index(-1), _name(nullptr), _raw(nullptr) {}
	public:
		
		bool valid() const { return _index > -1; }
		const char* name() const { return _name; }
		int index() const { return _index; }
		int width() const { return _raw->width; }
		int height() const { return _raw->height; }
		int left() const { return _raw->left; }
		int right() const { return _raw->right; }
		int bottom() const { return _raw->bottom; }
		int top() const { return _raw->top; }
		bool trasparent() const { return _raw->trasparent != 0; }
		bool smooth() const { return _raw->smooth !=0; }
		bool preload() const { return _raw->width!=0; }
		int mode() const { return _raw->mode; }
		int colcheck() const { return _raw->colcheck; }
		int original_x() const { return _raw->original_x; }
		int original_y() const { return _raw->original_y; }
		bool has_mask() const { return _masks.size() > 0; }
		const std::vector<const SpriteFrame*>& frames() { return _frames; }
		const std::vector<BitMask>& masks() { return _masks; }

	};
	class UndertaleFile {
	//	static unsigned int constexpr ChunkToInt(const char* name) {
		//	return name[0] << 24 | name[1] << 16 | name[2] << 8 | name[3];
	//	}
	//	static const int test = ChunkToInt("TXTR");
		enum class ChunkType : unsigned int {
			GEN8,
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
			_CMAX

		};
		struct OffsetList {
			uint32_t count;
			uint32_t offsets[];
			std::string toString() const;
		};
		struct Chunk {
			union {
				char name[4];
				uint32_t iname;
			};
			uint32_t size;
			uint32_t count;
			uint32_t offsets[];  // used for fast lookups
		};
		std::vector<const Chunk*> _chunks;
		std::vector<uint8_t> _data;
		std::unordered_map<std::string, const IName*> _lookup;
		std::unordered_map<uint32_t, std::unique_ptr<Sprite>> _spriteCache;
		bool internalParse();
		void internalReset();
		const Chunk* getChunk(ChunkType t) const { return _chunks[(uint32_t)t]; }
	public:

		UndertaleFile();
		bool loadFromFilename(const std::string& filename);
		bool loadFromData(const std::vector<uint8_t>& data); // we want it, so its a move
		//bool loadFromData(const std::vector<uint8_t*>& _data); // copy it instead
		const Sprite& LookupSprite(int index) const;
		const Sprite& LookupSprite(int index); // we cache here
		const Sprite& LookupSprite(const std::string& name) const;

	};
}
