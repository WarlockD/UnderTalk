#include "Global.h"
#include "json.hpp"
#include <UndertaleLib.h>
#include "gsprites.h"
#include <cstdarg>



using json = nlohmann::json;

using namespace sf;


#define USE_SSE 1



namespace fast {
#ifndef USE_SSE
	static uint32_t g_seed;
	void set_random_seed(uint32_t seed) {
		g_seed = seed;
	}
	uint32_t random() {
		g_seed = (214013 * g_seed + 2531011);
		return (g_seed >> 16) & 0x7FFF;
	}

#else
	__declspec(align(16)) static __m128i cur_seed;
	void set_random_seed(uint32_t seed) {
		cur_seed = _mm_set_epi32(seed, seed + 1, seed, seed + 1);
	}
	uint32_t random()
	{
		__declspec(align(16)) __m128i cur_seed_split;
		__declspec(align(16)) __m128i multiplier;
		__declspec(align(16)) __m128i adder;
		__declspec(align(16)) __m128i mod_mask;
		__declspec(align(16)) __m128i sra_mask;

		__declspec(align(16)) static const unsigned int mult[4] = { 214013, 17405, 214013, 69069 };
		__declspec(align(16)) static const unsigned int gadd[4] = { 2531011, 10395331, 13737667, 1 };
		__declspec(align(16)) static const unsigned int mask[4] = { 0xFFFFFFFF, 0, 0xFFFFFFFF, 0 };
		__declspec(align(16)) static const unsigned int masklo[4] = { 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF };

		adder = _mm_load_si128((__m128i*) gadd);
		multiplier = _mm_load_si128((__m128i*) mult);
		mod_mask = _mm_load_si128((__m128i*) mask);
		sra_mask = _mm_load_si128((__m128i*) masklo);
		cur_seed_split = _mm_shuffle_epi32(cur_seed, _MM_SHUFFLE(2, 3, 0, 1));

		cur_seed = _mm_mul_epu32(cur_seed, multiplier);
		multiplier = _mm_shuffle_epi32(multiplier, _MM_SHUFFLE(2, 3, 0, 1));
		cur_seed_split = _mm_mul_epu32(cur_seed_split, multiplier);
		cur_seed = _mm_and_si128(cur_seed, mod_mask);
		cur_seed_split = _mm_and_si128(cur_seed_split, mod_mask);
		cur_seed_split = _mm_shuffle_epi32(cur_seed_split, _MM_SHUFFLE(2, 3, 0, 1));
		cur_seed = _mm_or_si128(cur_seed, cur_seed_split);
		cur_seed = _mm_add_epi32(cur_seed, adder);

#ifdef COMPATABILITY
		__declspec(align(16)) __m128i sseresult;
		// Add the lines below if you wish to reduce your results to 16-bit vals...
		sseresult = _mm_srai_epi32(cur_seed, 16);
		sseresult = _mm_and_si128(sseresult, sra_mask);
		_mm_storeu_si128((__m128i*) result, sseresult);
		return;
#endif
		uint32_t result;
		//_mm_storeu_si128((__m128i*) result, cur_seed);
		_mm_storeu_si128((__m128i*)&result, cur_seed);
		return result;
	}
#endif
	class SetRandomSeed {
		static SetRandomSeed _setSeed;
		SetRandomSeed() {
			uint32_t seed = static_cast<unsigned int>(std::time(NULL));
			set_random_seed(seed);
			std::srand(seed); // just in case for a backup
		}
	};
	SetRandomSeed SetRandomSeed::_setSeed;
};



namespace util {
	Randomizer s_random;
	template<typename T> T random(T a, T b) { return s_random.rnd(a, b); }
	template<typename T> T random(T a) { return s_random.rnd(a); }
	template<> float random(float a) { return s_random.rnd<float>(a); }
	template<> float random(float a, float b) { return s_random.rnd<float>(a); }
};

void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale) {
	{
		float left = offset.x;
		float right = left + static_cast<float>(textRect.width);
		float top = offset.y;
		float bottom = top + static_cast<float>(textRect.height);

		vertices[0].position = Vector2f(left, top);
		vertices[1].position = Vector2f(left, bottom);
		vertices[2].position = Vector2f(right, top);
		vertices[3].position = Vector2f(right, bottom);
	}

	{
		float left = static_cast<float>(textRect.left);
		float right = left + textRect.width;
		float top = static_cast<float>(textRect.top);
		float bottom = top + textRect.height;

		vertices[0].texCoords = Vector2f(left, top);
		vertices[1].texCoords = Vector2f(left, bottom);
		vertices[2].texCoords = Vector2f(right, top);
		vertices[3].texCoords = Vector2f(right, bottom);
	}
	{
		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;
	}
	
}
void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color,  const sf::Vector2f& offset, const sf::Vector2f& scale) {
	float left = offset.x;
	float top = offset.y;
	float right = (left + textRect.width) * scale.x;
	float bottom = (top + textRect.height)* scale.y;

	float u1 = static_cast<float>(textRect.left);
	float v1 = static_cast<float>(textRect.top);
	float u2 = static_cast<float>(textRect.left + textRect.width);
	float v2 = static_cast<float>(textRect.top + textRect.height);

	vertices[0].position = sf::Vector2f(left, top);
	vertices[1].position = sf::Vector2f(right, top);
	vertices[2].position = sf::Vector2f(left, bottom);
	vertices[3].position = sf::Vector2f(left, bottom);
	vertices[4].position = sf::Vector2f(right, top);
	vertices[5].position = sf::Vector2f(right, bottom);

	vertices[0].texCoords = sf::Vector2f(u1, v1);
	vertices[1].texCoords = sf::Vector2f(u2, v1);
	vertices[2].texCoords = sf::Vector2f(u1, v2);
	vertices[3].texCoords = sf::Vector2f(u1, v2);
	vertices[4].texCoords = sf::Vector2f(u2, v1);
	vertices[5].texCoords = sf::Vector2f(u2, v2);

	vertices[0].color = color;
	vertices[1].color = color;
	vertices[2].color = color;
	vertices[3].color = color;
	vertices[4].color = color;
	vertices[5].color = color;
}
void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset) {
	MakeSpriteTriangleStrip(vertices, textRect, color, offset, Vector2f(1.0f, 1.0f));
}
void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset) {
	MakeSpriteTriangles(vertices, textRect, color, offset, Vector2f(1.0f, 1.0f));
}

void AppendSpriteTriangleStrip(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color,  const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.getVertexCount();
	vertices.resize(pos + 6);
	MakeSpriteTriangleStrip(&vertices[pos], textRect, color, offset, scale);
}

void AppendSpriteTriangleStrip(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color,  const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.size();
	vertices.resize(pos + 6);
	MakeSpriteTriangleStrip(&vertices[pos], textRect, color, offset, scale);
}

void AppendSpriteTriangles(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color,  const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.getVertexCount();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset,scale);
}

void AppendSpriteTriangles(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color,  const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.size();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset, scale);
}

struct FontInfo {
	std::string name;
	int size;
	std::map<int, sf::Glyph> glyphs;
	SharedTexture texture;
	IntRect frame;
};

struct SoundInfo {
	std::string filename;
	std::string name;
	float volume;
	float pan;
	int index;
};

Undertale::UndertaleFile res;
std::map<int, SoundInfo> _audiofiles; 

static SharedTexture images[20];
static std::map<int, FontInfo> fonts;

static bool loaded = false;
std::unordered_map<uint32_t, sf::Sprite> _spriteCache;

Undertale::UndertaleFile& GetUndertale() {
	return res;
}

void LoadUndertaleResources(const std::string& filename) {
	if (!loaded) {
		if (!res.loadFromFilename(filename)) {
			printf("Could not load data win");
			exit(-1);
		}
		loaded = true;

		for (auto& font : res.ReadAllfonts()) {
			FontInfo info;
			info.size = font.size();
			info.name = font.name().string();
			info.frame = IntRect(font.frame().x, font.frame().y, font.frame().width, font.frame().height);
			info.texture = Undertale::GetTexture(font.frame().texture_index);

			for (auto& glyph : font.glyphs()) {
				sf::Glyph g;
				g.advance = glyph.shift;
				auto rect = sf::IntRect(glyph.x, glyph.y, glyph.width, glyph.height);
				// glyph["offset"] // kind of important, but not sure where it goes
				g.textureRect = rect;
				g.bounds = FloatRect((float)0, (float)0, (float)rect.width, (float)rect.height);
				g.textureRect.top += info.frame.top;
				g.textureRect.left += info.frame.left;
				// g.bounds ignore for now
				info.glyphs[glyph.ch] = std::move(g);
			}
			int index = font.index();
			fonts.insert(std::pair<int, FontInfo>(index, info));
		}
	}
}





static std::unordered_map<uint32_t, std::weak_ptr<Texture>> simple_textures;
static std::unordered_map<uint32_t, std::unique_ptr<Image>> simple_images;

namespace Undertale {
	const sf::Image& GetTextureImage(int index) {
		auto& ptr = simple_images[index];
		if (!ptr) {
			Image* image = new Image;
			auto utexture = GetUndertale().LookupTexture(index);
			if (!image->loadFromMemory(utexture.data(), utexture.len())) {
				printf("Cannot load texture index %i", index);
				exit(1);
			}
			ptr.reset(image);
		}
		return *ptr.get();
	}
	SharedTexture GetTexture(uint32_t index) {
		std::shared_ptr<sf::Texture> texture = simple_textures[index].lock(); // I still don't get if make shared is better here or not?
		if (!texture) {
			texture = std::make_shared<sf::Texture>();
			if (!texture->loadFromImage(GetTextureImage(index))) {
				printf("Cannot load texture index %i", index);
				exit(1);
			}
			simple_textures[index] = texture;
		}
		return std::move(texture);
	}
\

	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index) {
		return fonts[font_index].glyphs;
	}
	int GetFontSize(int font_index) {
		return fonts[font_index].size;
	}
	const sf::Texture* GetFontTexture(int font_index) {
		return fonts[font_index].texture.get();
	}
	const std::string& LookupSound(int index) {
		if (_audiofiles.empty()) {
			std::fstream file("resources\\sounds.json", std::ios::in);
			json j;
			file >> j;
			for (auto sound : j) {
				SoundInfo si;
				int index = (int)sound["index"];
				si.index = index;
				si.filename = "resources\\sounds\\" + sound["filename"].get<std::string>();
				si.name = sound["name"].get<std::string>();
				si.volume = (float)sound["volume"];
				si.pan = (float)sound["pan"];
				_audiofiles.emplace(std::pair<int, SoundInfo>(index, std::move(si)));
			}
		}
		return _audiofiles[index].filename;
	}
}

#ifdef _DEBUG
// since we got to load this, lets try loading it at the end
#include <windows.h>

typedef void t_OutputDebugStringW(const wchar_t *);
typedef void t_OutputDebugStringA(const char *);

//t_OutputDebugStringW* OutputDebugStringW = nullptr;
//t_OutputDebugStringA* OutputDebugStringA = nullptr;
//#define OutputDebugString OutputDebugStringA

#endif

namespace console {
	/// The max length of CCLog message.
	static const int MAX_LOG_LENGTH = 16 * 1024;
	static Mutex consoleMutex;
	char _logbuffer[MAX_LOG_LENGTH];
	char _logFinalbuffer[MAX_LOG_LENGTH];
	void PrintMessage(const char* type, const char* message) {
		char* msg = _logFinalbuffer; // oldschool booy
		*msg++ = '[';
		while (*type) *msg++ = *type++;
		*msg++ = ']';
		*msg++ = ':';
		*msg++ = ' ';
		while (*message) *msg++ = *message++;
		*msg++ = '\r';
		*msg++ = '\n';
		*msg++ = 0;
		printf(_logFinalbuffer);
#if _DEBUG
		if (OutputDebugStringA == nullptr) {
			//	HINSTANCE dllHandle = LoadLibrary("art.dll");
			//	FindArtistType FindArtistPtr = NULL;

			//Load the dll and keep the handle to it
			//	dllHandle = 
			//	OutputDebugStringA
		}
		OutputDebugStringA(_logFinalbuffer);
#endif

	}
	void info(const char* format, ...) {
		consoleMutex.lock();
		va_list args;
		va_start(args, format);
		vsnprintf_s(_logbuffer, MAX_LOG_LENGTH, format, args);
		va_end(args);
		PrintMessage("INFO", _logbuffer);
		consoleMutex.unlock();
	}
	void error(const char * format, ...) {
		consoleMutex.lock();
		va_list args;
		va_start(args, format);
		vsnprintf_s(_logbuffer, MAX_LOG_LENGTH, format, args);
		va_end(args);
		PrintMessage("ERROR", _logbuffer);
		consoleMutex.unlock();
	}
}