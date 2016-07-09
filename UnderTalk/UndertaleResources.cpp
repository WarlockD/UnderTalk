#include "Global.h"
#include "json.hpp"
#include <UndertaleLib.h>
#include "gsprites.h"

using json = nlohmann::json;

using namespace sf;

namespace util {
	Randomizer s_random;
	template<typename T> T random(T a, T b) { return s_random.rnd(a, b); }
	template<typename T> T random(T a) { return s_random.rnd(a); }
	template<> float random(float a) { return s_random.rnd<float>(a); }
	template<> float random(float a, float b) { return s_random.rnd<float>(a); }
};

void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
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
void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
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

void AppendSpriteTriangleStrip(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.getVertexCount();
	vertices.resize(pos + 6);
	MakeSpriteTriangleStrip(&vertices[pos], textRect, color, offset, scale);
}

void AppendSpriteTriangleStrip(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.size();
	vertices.resize(pos + 6);
	MakeSpriteTriangleStrip(&vertices[pos], textRect, color, offset, scale);
}

void AppendSpriteTriangles(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.getVertexCount();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset,scale);
}

void AppendSpriteTriangles(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale) {
	size_t pos = vertices.size();
	vertices.resize(pos + 6);
	MakeSpriteTriangles(&vertices[pos], textRect, color, offset, scale);
}

struct FontInfo {
	std::string name;
	int size;
	std::map<int, sf::Glyph> glyphs;
	SharedTexture::TextureInfo texture;
	IntRect frame;
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

static SharedTexture images[20];
static std::map<int, FontInfo> fonts;

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





void Undertale::LoadAllFonts() {
	if (!loaded) {
		loaded = true;
		for (auto font : GetUndertale().ReadAllfonts()) {
			FontInfo info;
			info.size = font->size();
			info.name = font->name().string();
			info.frame = IntRect(font->frame()->x, font->frame()->y, font->frame()->width, font->frame()->height);
			info.texture = GetTexture(font->frame()->texture_index, info.frame);

			for (auto glyph : font->glyphs()) {
				sf::Glyph g;
				g.advance = glyph->shift;
				auto rect = sf::IntRect(glyph->x, glyph->y, glyph->width, glyph->height);
				// glyph["offset"] // kind of important, but not sure where it goes
				g.textureRect = rect;
				g.bounds = FloatRect(0, 0, rect.width, rect.height);
				g.textureRect.top += info.frame.top;
				g.textureRect.left += info.frame.left;
				// g.bounds ignore for now
				info.glyphs[glyph->ch] = std::move(g);
			}
			int index = font->index();
			printf("Font loaded (%i)'%s'\n", index, info.name.c_str());
			fonts.insert(std::pair<int, FontInfo>(index, info));
		}
	}
}


/*
class SharedTexture {
sf::Image _image;
std::unordered_map<sf::IntRect, std::weak_ptr<sf::Texture>> _textures;
public:
SharedTexture(const sf::Image& image) : _image(image) {}
SharedTexture(sf::Image&& image) : _image(image) {}
const sf::Image& getImage() const { return _image; }
// attempts to request a texture.  If we cannot load the whole thing, then we cut the texture
// int to a partial texture and give you that
bool requestTexture(sf::IntRect& frame, std::shared_ptr<sf::Texture>& texture);
};
*/
void SharedTexture::checkOpenGL() {
	assert(!_fullTexture);
	auto max_size = Texture::getMaximumSize();
	if (_image.getSize().x <= max_size || _image.getSize().y <= max_size) {
		TextureRef* ref = new TextureRef;
		if (!ref->texture.loadFromImage(_image)) {
			printf("Could not cut up texture\r\n");
			throw std::exception("Ugh");
		}
		ref->frame = IntRect(0, 0, _image.getSize().x, _image.getSize().y);
		_fullTexture.reset(ref);
	}
}
SharedTexture::TextureInfo  SharedTexture::requestTexture(const sf::IntRect& frame) {
	if (_fullTexture) return TextureInfo(this, _fullTexture,frame);
	IntRect rect(0, 0, frame.width, frame.height);
	auto it = _textures.find(frame);
	if (it != _textures.end() && !it->second.expired()) return TextureInfo(this, it->second.lock(), rect);
	else {
		TextureRef* ref = new TextureRef;	// we have to look up the texture and split it up
		if(!ref->texture.loadFromImage(_image, frame)) {
			printf("Could not cut up texture\r\n");
			throw std::exception("Ugh");
		}
		ref->frame = frame;
		std::shared_ptr<TextureRef> shared = std::shared_ptr<TextureRef>(ref);
		_textures.emplace(frame, shared);
		return TextureInfo(this, shared, rect); 
	}
}
std::shared_ptr<sf::Texture> SharedTexture::requestTexture(const std::initializer_list<sf::IntRect>& frames) {
	auto max_size = Texture::getMaximumSize();
	if (_image.getSize().x <= max_size && _image.getSize().y <= max_size) {
		// we don't have to do anything, just return the existing texture in the cache
		//return requestTexture(*frames.begin()).second;
	}
	// just throw something here sigh
	throw std::exception("do something here");
	//std::vector < std::pair<sf::IntRect, std::shared_ptr<sf::Texture>>> _frames;
}
namespace Undertale {
	SharedTexture::TextureInfo GetTexture(int index, const sf::IntRect& rect) {
		if (images[index].getImage().getSize() == Vector2u()) { // if zero then we have to load it
			sf::Image image;
			auto utexture = GetUndertale().LookupTexture(index);
			if (!image.loadFromMemory(utexture.data(), utexture.len())) {
				printf("Cannot load texture index %i", index);
				exit(1);
			}
			images[index].setImage(std::move(image));
		}
		auto& image = images[index];
		return image.requestTexture(rect);
	}


	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index) {
		return fonts[font_index].glyphs;
	}
	int GetFontSize(int font_index) {
		return fonts[font_index].size;
	}
	const sf::Texture* GetFontTexture(int font_index) {
		return fonts[font_index].texture.getTexture();
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