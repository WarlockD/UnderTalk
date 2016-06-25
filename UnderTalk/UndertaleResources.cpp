#include "Global.h"
#include "json.hpp"
using json = nlohmann::json;

using namespace sf;
struct FontInfo {
	std::string name;
	int size;
	std::map<int, sf::Glyph> glyphs;
	sf::Texture* texture;
};
struct SoundInfo {
	std::string filename;
	std::string name;
	float volume;
	float pan;
	int index;
};
std::map<int, SoundInfo> _audiofiles;
static std::map<int, FontInfo> fonts;
static sf::Texture* textures[15] = { nullptr, nullptr };
static sf::Image* images[15] = { nullptr, nullptr };
static bool loaded = false;
//const char* filepath = "C:\\Users\\Paul\\Documents\\GitHub\\SFML\\examples\\pong\\resources\\textures\\texture_";
void LoadAllTextures() {
	if (textures[0] == nullptr) {
		for (int i = 0; i < 15; i++) {
			if (textures[i] != nullptr) continue;
			std::string filename("resources/textures/texture_");
			filename += std::to_string(i);
			filename += ".png";

			Image* image = new Image;
			if (image->loadFromFile(filename)) {
				images[i] = image;
				sf::Texture* texture = new sf::Texture();
				if (!texture->loadFromImage(*image)) {
					printf("Cannot load texture '%s'", filename.c_str());
					exit(1);
				}
			}
			else {
				printf("Cannot load image '%s'", filename.c_str());
				exit(1);
			}
		}
	}
}

void Undertale::LoadAllFonts() {
	if (!loaded) {
		loaded = true;
		//	LoadAllTextures();
		if (textures[6] == nullptr) {
			sf::Texture* texture = new sf::Texture();
			if (!texture->loadFromFile("resources/textures/texture_6.png")) {
				printf("Cannot load font texture");
				exit(1);
			}
			textures[6] = texture;
		}
		std::fstream file("resources\\fonts.json", std::ios::in);
		json j;
		file >> j;
		for (auto font : j) {
			auto frame = font["Frame"];
			sf::IntRect frameRect(frame["X"], frame["Y"], frame["Width"], frame["Height"]);
			FontInfo info;
			info.size = font["Size"];
			info.name = font["name"].dump();
			info.texture = textures[frame["Texture_Index"]];
			for (auto glyph : font["Glyphs"]) {
				sf::Glyph g;
				g.advance = glyph["shift"];
				auto rect = sf::IntRect(glyph["x"], glyph["y"], glyph["width"], glyph["height"]);
				// glyph["offset"] // kind of important
				g.textureRect = rect;

				g.bounds = FloatRect(0, 0, rect.width, rect.height);
				g.textureRect.top += frameRect.top;
				g.textureRect.left += frameRect.left;
				// g.bounds ignore for now
				info.glyphs[glyph["ch"]] = std::move(g);
			}
			int index = (int)font["index"];
			printf("Font loaded (%i)'%s'\n", index, info.name.c_str());
			fonts.insert(std::pair<int, FontInfo>(index, info));
		}
	}

}


namespace Undertale {
	const sf::Texture& GetTexture(int index) {
		return *textures[index];
	}
	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index) {
		return fonts[font_index].glyphs;
	}
	int GetFontSize(int font_index) {
		return fonts[font_index].size;
	}
	const sf::Texture* GetFontTexture(int font_index) {
		return fonts[font_index].texture;
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