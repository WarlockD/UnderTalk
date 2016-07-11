#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <functional>

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
	class Resource {
	protected:
		static constexpr ChunkType ResType = ChunkType::BAD;
		String _name;
		int _index;
		friend class UndertaleFile;
	public:
		Resource() : _name(), _index(-1) {}
		~Resource() {}
		ChunkType type() const { return ResType; }
		virtual size_t hash() const { return _name.hash(); }
		virtual bool valid() const { return index() >= 0; }
		virtual int index() const { return _index; }
		virtual const String& name() const { return _name; }
		inline bool operator==(const Resource& other) const { return _name == other._name; } // its name will be unique cause the pointer is
	};
	template<class T> class OffsetVectorIt;
	template<class T> class OffsetVector {
		const uint8_t* _data;
		struct Record {
			uint32_t count;
			uint32_t offsets[1];
			Record(Record const &) = delete;           // undefined
			Record& operator=(Record const &) = delete;  // undefined
		};
		const Record* _rec;
	public:
		typedef OffsetVectorIt<T> const_iterator;
		typedef const_iterator iterator;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		OffsetVector() :_data(nullptr), _rec(nullptr) {}
		OffsetVector(const uint8_t* data, size_t offset) : _data(data), _rec(reinterpret_cast<const Record*>(data + offset)) {}
		OffsetVector(const uint8_t* data, const uint8_t* rec) : _data(data), _rec(reinterpret_cast<const Record*>(rec)) {}
		const T& at(uint32_t i) const { return *reinterpret_cast<const T*>(_data + _rec->offsets[i]); }
		const T& operator[](uint32_t i) const {return at(i);}
		
		OffsetVectorIt<T> begin() const;
		OffsetVectorIt<T>  end() const;
		size_t size() const { return _rec->count; }
		typedef std::initializer_list<T> test_t;
	};
	template<class T> class OffsetVectorIt  {
		const OffsetVector<T>& _vec;
		size_t _pos;
	public:
		OffsetVectorIt(const OffsetVector<T>& vec, size_t pos) : _vec(vec), _pos(pos) {}
		OffsetVectorIt(const OffsetVector<T>& vec) : _vec(vec), _pos(0) {}
		OffsetVectorIt& operator++() { _pos++; return *this; }
		OffsetVectorIt& operator--() { _ pos--; return *this; }
		const T& operator*() const { return _vec[_pos]; }
		bool operator==(const OffsetVectorIt& other) const { return _pos == other._pos; }
		bool operator!=(const OffsetVectorIt& other) const { return _pos != other._pos; }
	};
	

	template<class T> OffsetVectorIt<T> OffsetVector<T>::begin() const { return OffsetVectorIt<T>(*this, (size_t)0); }
	template<class T>  OffsetVectorIt<T> OffsetVector<T>::end() const { return OffsetVectorIt<T>(*this, size()); }
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
	struct std::hash<const Undertale::Resource*> {
		std::size_t operator()(const Undertale::Resource  *r) const { return r->hash(); }
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
		unsigned short width;
		unsigned short height;
		short offset_x;
		short offset_y;
		unsigned short crop_width;
		unsigned short crop_height;
		unsigned short original_width;
		unsigned short original_height;
		short texture_index;
		bool valid() const { return texture_index != -1; }
		SpriteFrame() : x(0), y(0), width(0), height(0), offset_x(0), offset_y(0), crop_width(0), crop_height(0), original_width(0), original_height(0), texture_index(-1) {}
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
		const uint8_t* data() const { return _data; }
		size_t len() const { return _len; }
	};
	class Background : public Resource {
		friend class UndertaleFile;
	protected:
		static constexpr ChunkType ResType = ChunkType::BGND;
		struct RawBackground {
			uint32_t name_offset;
			uint32_t trasparent;
			uint32_t smooth;
			uint32_t preload;
			uint32_t frame_offset;
		};
		const RawBackground* _raw;
		typedef RawBackground RawResourceType;
		const SpriteFrame* _frame;
	public:
		Background() : Resource(), _raw(nullptr), _frame(nullptr) {}
		bool trasparent() const { return _raw->trasparent != 0; }
		bool smooth() const { return _raw->smooth != 0; }
		bool preload() const { return _raw->preload != 0; }
		const SpriteFrame& frame() const { return *_frame; }
	};
	class Room : public Resource {
		friend class UndertaleFile;
	protected:
		static constexpr ChunkType ResType = ChunkType::ROOM;
	public:
		struct View {
			int visible;
			int x;
			int y;
			int width;
			int height;
			int port_x;
			int port_y;
			int port_width;
			int port_height;
			int border_x;
			int border_y;
			int speed_x;
			int speed_y;
			int view_index;
		};
		struct Background {
			int visible;// bool
			int foreground;// bool
			int background_index;// bool
			int x;
			int y;
			int tiled_x;
			int tiled_y;
			int speed_x;
			int speed_y;
			int strech; // bool
		};
		struct Object {
			int x;
			int y;
			int object_index;
			int id;
			int code_offset;
			float scale_x;
			float scale_y;
			int color;
			float rotation;
		};
		struct Tile {
			int x;
			int y;
			int background_index;
			int offset_x;
			int offset_y;
			int width;
			int height;
			int depth;
			int id;
			float scale_x;
			float scale_y;
			int blend; // color value
		};
	protected:
		struct RawRoom {
			int name_offset;
			int caption_offset;
			int width;
			int height;
			int speed;
			int persistent;
			int color;
			int show_color;
			int code_offset;
			int flags;
			int background_offset;
			int view_offset;
			int object_offset;
			int tiles_offset;
		};
		const RawRoom* _raw;
		typedef RawRoom RawResourceType;
		String _caption;
		OffsetVector<View> _views;
		OffsetVector<Background> _backgrounds;
		OffsetVector<Object> _objects;
		OffsetVector<Tile> _tiles;
	public:
		Room() : Resource(), _raw(nullptr) {}
		String caption() const { return _caption; }
		int width() const { return _raw->width; }
		int height() const { return _raw->height; }
		int speed() const { return _raw->speed; }
		bool persistent() const { return _raw->persistent!=0; }
		int color() const { return _raw->color; }
		bool show_color() const { return _raw->show_color!=0; }
		int code_offset() const { return _raw->code_offset; }
		bool enable_views() const { return (_raw->flags & 1) != 0; }
		bool view_clear_screen() const { return (_raw->flags & 2) != 0; }
		bool clear_display_buffer() const { return (_raw->flags & 14) != 0; }
		const OffsetVector<View>& views() const { return _views; }
		const OffsetVector<Background>& backgrounds() const { return _backgrounds; }
		const OffsetVector<Object>& objects() const { return _objects; }
		const OffsetVector<Tile>& tiles() const { return _tiles; }
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
		};
		friend class UndertaleFile;
		const RawFont* _raw;
		typedef RawFont RawResourceType;
		const SpriteFrame* _frame;
		String _description;
		OffsetVector<Glyph> _glyphs;
	public:
		Font() : Resource(), _raw(nullptr), _frame(nullptr), _description() {}
		int size() const { return _raw->size; }
		bool bold() const { return _raw->bold!=0; }
		bool italic() const { return _raw->italic!=0; }
		bool antiAlias() const { return ((_raw->flags >> 24) & 0xFF)!=0; }
		int charSet() const { return (_raw->flags >> 16) & 0xFF; }
		uint16_t firstChar() const { return (_raw->flags) & 0xFFFF; }
		uint16_t lastChar() const { return _raw->lastChar; }
		const SpriteFrame& frame() const { return *_frame; }
		float scaleWidth() const { return _raw->scale_width; }
		float scaleHeight() const { return _raw->scale_height; }
		const OffsetVector<Glyph>& glyphs() const { return _glyphs; }
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
		};
		#pragma pack()
		typedef RawSprite RawResourceType;
		const RawSprite* _raw;
		OffsetVector<SpriteFrame> _frames;
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
		const OffsetVector<SpriteFrame>& frames() const { return _frames; }
		const std::vector<BitMask>& masks() const { return _masks; }
	};
	class UndertaleFile {
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
			ResourceKey() : _index(0), _type(ChunkType::BAD) {}
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
		// lookup by index
		// lookup for name
		std::unordered_map<String, ResourceKey> _nameCache;
		// all strings hash
		std::unordered_set<const UndertaleString*> _strings; // the strings are directly mapped to the loaded data.win


		bool internalParse();
		void internalReset();
		const Chunk* getChunk(ChunkType t) const { return _chunks[(uint32_t)t]; }
		template<class T> const uint8_t* preCreateResorce(int index, T&res) const;
		template<class T> T&& createResource(int index) const { return T();  }
		template<> Font&& createResource(int index) const;
		template<> Sprite&& createResource(int index) const;
		template<> Background&& createResource(int index) const;
		template<> Room&& createResource(int index) const;


		String getUndertaleString(int offset) const;
		template<class C> void fillList(size_t offset, std::vector<const C*>& list) const;
		template<class C, class P> void fillList(size_t offset, std::vector<const C*>& list, P pred) const;
	public:

		UndertaleFile();
		bool isLoaded() const { return _data.size() > 0; }
		bool loadFromFilename(const std::string& filename);
		bool loadFromData(const std::vector<uint8_t>& data); // we want it, so its a move
		//bool loadFromData(const std::vector<uint8_t*>& _data); // copy it instead

		
		Sprite&& LookupSprite(int index);
		Room&& LookupRoom(int index);
		Background&& LookupBackground(int index);
		/// Returns the raw location of the texture, its a png so hopefuly you have something that can
		/// read it
		Texture&& LookupTexture(int index) const;
		std::vector<Font>&& ReadAllfonts();

	};
}
