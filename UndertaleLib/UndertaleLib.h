#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>

#ifdef UNDERLIBRARY_DLL
#ifdef UNDERLIBRARY_EXPORTS
#define UNDERLIBRARY_API __declspec(dllexport) 
#else
#define UNDERLIBRARY_API __declspec(dllimport) 
#endif
#else
#define UNDERLIBRARY_API
#endif

namespace Undertale {

	inline size_t simple_hash(const char *str)
	{
		size_t hash = 5381;
		int c;
		while (c = (unsigned char)(*str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash;
	}
	inline size_t simple_hash(const char *str, size_t len)
	{
		size_t hash = 5381;
		while (len > 0) {
			int c = *str++;
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			len--;
		}
		return hash;
	}
#pragma pack(push, 1)
	struct UndertaleString {
		uint32_t length;
		const char u_str[1];
		UndertaleString& operator=(UndertaleString const &) = delete;  // no move constructor
		UndertaleString(UndertaleString const &) = delete;           // no move constructor
		bool operator==(const UndertaleString& other) const { return &u_str[0] == &other.u_str[0]; } // always equal
		bool operator!=(const UndertaleString& other) const { return !(*this == other); }
	};
#pragma pack(pop)
	// Container for strings simple pointer strings.  Used for comparing and matching
	class String {
		const char* _c_str;
		size_t _len;
		friend class UndertaleFile;
	public:	
		String() :  _c_str(""), _len(0) {  }
		String(const char* str) :  _c_str(str), _len(strlen(str)) {}
		String(const char* str, size_t len) :  _c_str(str), _len(len) {}
	
		String& operator=(const char* str) {
			_c_str = str;
			_len = strlen(str);
			return *this;
		}
		const char* c_str() const { return   _c_str; }
		std::string string() const { return std::string(_c_str,_len); }
		size_t length() const { return _len; }
		size_t hash() const { return Undertale::simple_hash(_c_str, _len); }
		bool operator==(const String& other) const { return _c_str == other._c_str || (length() == other.length() && std::memcmp(c_str(), other.c_str(), length()) == 0); }
		bool operator!=(const String& other) const { return !(*this == other); }
		bool operator==(const std::string& other) const { return  length() == other.length() && std::memcmp(c_str(), other.c_str(), length()) == 0; }
		bool operator!=(const std::string& other) const { return !(*this == other); }
		bool operator==(const char* other) const { return _c_str == other || strncmp(_c_str,other,_len) == 0; }
		bool operator!=(const char* other) const { return !(*this == other); }
	};


	enum class ChunkType : unsigned int {
		BAD=0,
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
		STRG,
		_CMAX
	};
	class  Resource {
	protected:
		static constexpr ChunkType ResType = ChunkType::BAD;
		String _name;
		int _index;
		friend class UndertaleFile;
		
	public:
		Resource() : _name(), _index(-1) {}
		ChunkType type() const { return ResType; }
		virtual size_t hash() const { return _name.hash(); }
		virtual bool valid() const { return index() >= 0; }
		virtual int index() const { return _index; }
		virtual const String& name() const { return _name; }
		inline bool operator==(const Resource& other) const { return _name == other._name; } // its name will be unique cause the pointer is
		virtual ~Resource() {}
	};
}

namespace std {
	template <>
	struct std::hash<Undertale::Resource>
	{
		std::size_t operator()(const Undertale::Resource& r) const { return r.hash(); }
	};
	template <>
	struct std::hash<Undertale::String>
	{
		std::size_t operator()(const Undertale::String& r) const { return Undertale::simple_hash(r.c_str(), r.length()); }
	};
	template <>
	struct std::hash<Undertale::UndertaleString>
	{
		std::size_t operator()(const Undertale::UndertaleString& r) const { return Undertale::simple_hash(r.u_str, r.length); }
	};
	template <>
	struct std::hash<Undertale::UndertaleString*>
	{
		std::size_t operator()(const Undertale::UndertaleString* r) const { return Undertale::simple_hash(r->u_str, r->length); }
	};
	template<>
	struct std::hash<Undertale::Resource const *> {
		std::size_t operator()(Undertale::Resource const *r) const { return r->hash(); }
	};
	template<>
	struct std::equal_to<Undertale::Resource const *> {
		bool operator()(Undertale::Resource const *l, Undertale::Resource const *r) const { return *l == *r; }
	};
}
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
	class Texture {
		const uint8_t* _data;
		size_t _len;
	public:
		Texture() : _data(nullptr), _len(0) {}
		Texture(const uint8_t* data, size_t len) :_data(data), _len(len) {}
		const uint8_t* data() const { return _data;  }
		size_t len() const { return _len; }
	};
	class Font : public Resource {
	protected:
		static constexpr ChunkType ResType = ChunkType::FONT;
	public:
		struct Glyph {
			short ch;
			short x;
			short y;
			short width;
			short height;
			short shift;
			short offset;
		};
	private:
		struct RawFont {
			int name_offset;
			int description_offset;
			int size;
			int bold;
			int italic;
			int flags; // (antiAlias | CharSet | firstchar)
			int lastChar;
			uint32_t frame_offset;
			float scale_width;
			float scale_height;
			uint32_t glyph_count;
			int glyph_offsets[1];
			RawFont(RawFont const &) = delete;           // undefined
			RawFont& operator=(RawFont const &) = delete;  // undefined
		};
		friend class UndertaleFile;
		const RawFont* _raw;
		const SpriteFrame* _frame;
		String _description;
		std::vector<const Glyph*> _glyphs;
	public:
		Font() : Resource(), _raw(nullptr), _frame(nullptr), _description() {}
		int size() const { return _raw->size; }
		bool bold() const { return _raw->bold!=0; }
		bool italic() const { return _raw->italic!=0; }
		bool antiAlias() const { return ((_raw->flags >> 24) & 0xFF)!=0; }
		int charSet() const { return (_raw->flags >> 16) & 0xFF; }
		uint16_t firstChar() const { return (_raw->flags) & 0xFFFF; }
		uint16_t lastChar() const { return _raw->lastChar; }
		const SpriteFrame* frame() const { return _frame; }
		float scaleWidth() const { return _raw->scale_width; }
		float scaleHeight() const { return _raw->scale_height; }
		const std::vector<const Glyph*>& glyphs() const { return _glyphs; }
	};


	class Sprite : public Resource {
	protected:
		static constexpr ChunkType ResType = ChunkType::SPRT;
	private:
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
			uint32_t frame_count;
			int frame_offsets[1];
			RawSprite(RawSprite const &) = delete;           // undefined
			RawSprite& operator=(RawSprite const &) = delete;  // undefined
			// int frame_count, int[] offsets
			// int mask_offsets int[] offsets
		};
		#pragma pack()
		const RawSprite* _raw;
		std::vector<const SpriteFrame*> _frames;
		std::vector<BitMask> _masks;
		friend class UndertaleFile;
	public:
		Sprite() :Resource(), _raw(nullptr) {}
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
		int origin_x() const { return _raw->original_x; }
		int origin_y() const { return _raw->original_y; }
		bool has_mask() const { return _masks.size() > 0; }
		const std::vector<const SpriteFrame*>* frames() const { return &_frames; }
		const std::vector<BitMask>& masks() const { return _masks; }

	};
	class UndertaleFile {
	
	//	static unsigned int constexpr ChunkToInt(const char* name) {
		//	return name[0] << 24 | name[1] << 16 | name[2] << 8 | name[3];
	//	}
	//	static const int test = ChunkToInt("TXTR");
	
		struct OffsetList {
			uint32_t count;
			uint32_t offsets[1];
			OffsetList(OffsetList const &) = delete;           // undefined
			OffsetList& operator=(OffsetList const &) = delete;  // undefined
			std::string toString() const;
		};
		struct Chunk {
			union {
				char name[4];
				uint32_t iname;
			}; 
			uint32_t size;
			uint32_t count;
			uint32_t offsets[1];  // used for fast lookups
			Chunk(Chunk const &) = delete;           // undefined
			Chunk& operator=(Chunk const &) = delete;  // undefined
		};

		std::vector<const Chunk*> _chunks;
		std::vector<uint8_t> _data;
		class ResourceKey {
			int _index;
			ChunkType _type;
		public:
			ResourceKey(ChunkType t, int index) : _index(index), _type(t) {}
			inline int index() const { return _index; }
			inline ChunkType type() const { return _type; }
			bool operator==(const ResourceKey& k)  const { return type() == k.type() && index() == k.index(); }
		};
		struct ResourceKeyHasher
		{
			std::size_t operator()(const ResourceKey& r) const { return (uint8_t)r.type() << 24 || r.index(); }
		};
		// Contains the managed pointer
		std::unordered_set<std::unique_ptr<Resource>> _cache;
		// lookup by index
		std::unordered_map<ResourceKey, const Resource*, ResourceKeyHasher> _indexCache;
		// lookup for name
		std::unordered_map<String, const Resource*> _nameCache;
		// all strings hash
		std::unordered_set<const UndertaleString*> _strings; // the strings are directly mapped to the loaded data.win

		typedef std::unordered_map<ResourceKey, const Resource*, ResourceKeyHasher>::iterator CacheIterator;

		typedef std::unordered_map<std::string, const Resource*>::iterator NameCacheIterator;

		bool internalParse();
		void internalReset();
		const Chunk* getChunk(ChunkType t) const { return _chunks[(uint32_t)t]; }
		template<class T> T* createResource(int index) const { return nullptr;  }
		template<> Font* createResource(int index) const;
		template<> Sprite* createResource(int index) const;

		template<class T> const T* cascheReadResorce(int index);
		String getUndertaleString(int offset) const;

	public:

		UndertaleFile();
		bool loadFromFilename(const std::string& filename);
		bool loadFromData(const std::vector<uint8_t>& data); // we want it, so its a move
		//bool loadFromData(const std::vector<uint8_t*>& _data); // copy it instead
		template<class T> const T* LookupResource(int index);
		const Sprite* LookupSprite(int index);
		/// Returns the raw location of the texture, its a png so hopefuly you have something that can
		/// read it
		Texture LookupTexture(int index) const;
		std::vector<const Font*> ReadAllfonts();

	};
}
