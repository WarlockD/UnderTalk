#include "Global.h"
#include "json.hpp"
#include "gsprites.h"

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



void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const  sf::Vector2f& offset, const sf::Vector2f& scale) {
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
void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset) {
	MakeSpriteTriangles(vertices, textRect, color, offset, Vector2f(1.0f, 1.0f));
}

void AppendSpriteTriangles(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color, const  sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.getVertexCount();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset,scale);
}

void AppendSpriteTriangles(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color, const  sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.size();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset, scale);
}

struct FontInfo {
	std::string name;
	int size;
	std::map<int, sf::Glyph> glyphs;
	GSpriteFrame frame;
};

struct SoundInfo {
	std::string filename;
	std::string name;
	float volume;
	float pan;
	int index;
};

Undertale::UndertaleFile* res = nullptr;
std::map<int, SoundInfo> _audiofiles; 

static std::unique_ptr<sf::Image> images[20];
static std::map<int, FontInfo> fonts;
static std::unordered_map<size_t, Texture*> textures;

static bool loaded = false;
std::unordered_map<uint32_t, sf::Sprite> _spriteCache;

void LoadUndertaleResources(const std::string& filename) {
	if (res == nullptr) {
		res = new Undertale::UndertaleFile;
		if (!res->loadFromFilename(filename)) {
			printf("Could not load data win");
			exit(-1);
		}
	}
}
Undertale::UndertaleFile& GetUndertale() {
	assert(res != nullptr);
	return *res;
}


const sf::Sprite& GetUndertaleSprite(int index, int frame ) {
	uint32_t pos = (index & 0xFFFF << 16) | (frame & 0xFFFF);

	auto it = _spriteCache.find(pos);
	if (it != _spriteCache.end()) return it->second;
	auto rsprite = res->LookupSprite(index);
	auto rframe = rsprite->frames()->at(frame);

	Sprite sprite;
	sprite.setTexture(*textures[rframe->texture_index]);
	sprite.setTextureRect(IntRect(rframe->x, rframe->y, rframe->width, rframe->height));
	sprite.setPosition(rframe->offset_x, rframe->offset_y);
	_spriteCache.emplace(pos, sprite);
	return _spriteCache[index];
}



void Undertale::LoadAllFonts() {
	if (!loaded) {
		loaded = true;
		for (auto font : GetUndertale().ReadAllfonts()) {
			FontInfo info;
			info.size = font->size();
			info.name = font->name().string();
			info.frame.setFrame(font->frame());// = Undertale::GetTexture(font->frame().texture_index);
			for (auto glyph : font->glyphs()) {
				sf::Glyph g;
				g.advance = glyph->shift;
				auto rect = sf::IntRect(glyph->x, glyph->y, glyph->width, glyph->height);
				// glyph["offset"] // kind of important, but not sure where it goes
				g.textureRect = rect;
				g.bounds = FloatRect(0, 0, (float)rect.width, (float)rect.height);
				g.textureRect.top += info.frame.getTextureRect().top;
				g.textureRect.left += info.frame.getTextureRect().left;
				// g.bounds ignore for now
				info.glyphs.emplace(glyph->ch, g); 
			}
			int index = font->index();
		//	printf("Font loaded (%i)'%s'\n", index, info.name.c_str());
			fonts.insert(std::pair<int, FontInfo>(index, info));
		}
	}
}

void Debug_PreloadAllImages() {
	for (size_t i = 0; i < 16; i++) Undertale::GetTextureImage(i);
}
namespace Undertale {
	
	const sf::Image* GetTextureImage(int index) {
		if (!images[index]) {
			sf::Image* image = new sf::Image;
			auto utexture = GetUndertale().LookupTexture(index);
			if (!image->loadFromMemory(utexture.data(), utexture.len())) {
				printf("Cannot load texture index %i", index);
				exit(1);
			}
			images[index] = std::unique_ptr<Image>(image);
		}
		return images[index].get();
	}
	const sf::Texture* GetTexture(int index) {
		auto it = textures.find(index);
		if (it != textures.end()) return it->second;
		const sf::Image* image = Undertale::GetTextureImage(index);
		assert(image != nullptr);
		sf::Texture* texture = new sf::Texture;
		if (!texture->loadFromImage(*image) || texture->getSize() != image->getSize()) {
			printf("Cannot load texture index %i", index);
			exit(1);
		}
		textures.emplace(index, texture);
		return texture;
	}

	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index) {
		return fonts[font_index].glyphs;
	}
	int GetFontSize(int font_index) {
		return fonts[font_index].size;
	}
	const sf::Texture* GetFontTexture(int font_index) {
		return fonts[font_index].frame.getTexture();
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