#include "obj_writer.h"
#include "json.hpp"
using json = nlohmann::json;

using namespace sf;


// from drawables/ Text.cpp
namespace {
	// Add an underline or strikethrough line to the vertex array
	void addLine(sf::VertexArray& vertices, float lineLength, float lineTop, const sf::Color& color, float offset, float thickness, float outlineThickness = 0)
	{
		float top = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
		float bottom = top + std::floor(thickness + 0.5f);

		vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness, top - outlineThickness), color, sf::Vector2f(1, 1)));
		vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, top - outlineThickness), color, sf::Vector2f(1, 1)));
		vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness, bottom + outlineThickness), color, sf::Vector2f(1, 1)));
		vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness, bottom + outlineThickness), color, sf::Vector2f(1, 1)));
		vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, top - outlineThickness), color, sf::Vector2f(1, 1)));
		vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, bottom + outlineThickness), color, sf::Vector2f(1, 1)));
	}

	// Add a glyph quad to the vertex array
	void addGlyphQuad(sf::VertexArray& vertices, sf::Vector2f position, const sf::Color& color, const sf::Glyph& glyph)
	{
		float left = position.x;
		float top = position.y;
		float right = left+glyph.bounds.width;
		float bottom = top +glyph.bounds.height;

		float u1 = static_cast<float>(glyph.textureRect.left);
		float v1 = static_cast<float>(glyph.textureRect.top);
		float u2 = static_cast<float>(glyph.textureRect.left + glyph.textureRect.width);
		float v2 = static_cast<float>(glyph.textureRect.top + glyph.textureRect.height);

		vertices.append(sf::Vertex(sf::Vector2f(left   , top ), color, sf::Vector2f(u1, v1)));
		vertices.append(sf::Vertex(sf::Vector2f(right   , top), color, sf::Vector2f(u2, v1)));
		vertices.append(sf::Vertex(sf::Vector2f(left   , bottom ), color, sf::Vector2f(u1, v2)));
		vertices.append(sf::Vertex(sf::Vector2f(left   ,  bottom ), color, sf::Vector2f(u1, v2)));
		vertices.append(sf::Vertex(sf::Vector2f(right   ,  top ), color, sf::Vector2f(u2, v1)));
		vertices.append(sf::Vertex(sf::Vector2f(right  , bottom ), color, sf::Vector2f(u2, v2)));
	}
}



struct FontInfo {
	std::string name;
	int size;
	std::map<int, sf::Glyph> glyphs;
	sf::Texture* texture;
};
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

void OBJ_WRITER::LoadAllFonts() {
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
struct SoundInfo {
	std::string filename;
	std::string name;
	float volume;
	float pan;
	int index;
};
std::map<int, SoundInfo> _audiofiles;

namespace Undertale {
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


/*
void OBJ_WRITER::AddText(const std::string& str) {
	_text = str;
	_quads.clear();
	_texture = Undertale::GetFontTexture(1);
	_size = Undertale::GetFontSize(1);
	auto glyphs = Undertale::GetFontGlyphs(1);

	Vector2f pos;
	_sprites.resize(str.length());
	for (int i = 0; i < str.length(); i++) {
		char c = str[i];
		auto& g = glyphs[c];
		addGlyphQuad(_quads, pos, Color::White, g);
		pos.x += g.advance;
	}

}

struct TextSetup {
	int myfont;
	sf::Color color;
	sf::FloatRect writing; // bounds
	int shake;
	int textspeed;
	int txtsound;
	int spacing;
	int vspacing;
};
*/
void  OBJ_WRITER::SetTextType(int type) {
	_texture = Undertale::GetFontTexture(6);
	auto pos = getPosition();
	setup = { 1, Color::White, FloatRect(pos.x + 20, pos.y + 20, 290, 0), 1,1,94,16,32 };
	Reset();
	if (!_textSoundBuffer.loadFromFile(Undertale::LookupSound(setup.txtsound))) {
		printf("Could not load sound");
	}
	else {
		_textSound.setBuffer(_textSoundBuffer);
	}
	/*
	if (global.typer == 1) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, self.x + global.idealborder[1] - 55, 1, 1, 94, 16, 32);
	if (global.typer == 2) script_execute(149, 4, 0, self.x, self.y, self.x + 190, 43, 2, 95, 9, 20);
	if (global.typer == 3) script_execute(149, 7, 8421376, self.x, self.y, self.x + 100, 39, 3, 95, 10, 10);
	if (global.typer == 4) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 101, 8, 18);
	if (global.typer == 5) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 95, 8, 18);
	if (global.typer == 6) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 1, 97, 9, 20);
	if (global.typer == 7) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 2, 2, 98, 9, 20);
	if (global.typer == 8) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 1, 101, 9, 20);
	if (global.typer == 9) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 97, 8, 18);
	if (global.typer == 10) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 96, 8, 18);
	if (global.typer == 11) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 94, 9, 18);
	if (global.typer == 12) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 1, 3, 99, 10, 20);
	if (global.typer == 13) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 2, 4, 99, 11, 20);
	if (global.typer == 14) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 3, 5, 99, 14, 20);
	if (global.typer == 15) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 10, 99, 18, 20);
	if (global.typer == 16) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 1.2, 2, 98, 8, 18);
	if (global.typer == 17) script_execute(149, 8, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 88, 8, 18);
	if (global.typer == 19) global.typer = 18;
	if (global.typer == 18) script_execute(149, 9, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 87, 11, 18);
	if (global.typer == 20) script_execute(149, 5, 0, self.x, self.y, self.x + 200, 0, 2, 98, 25, 20);
	if (global.typer == 21) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 4, 96, 10, 18);
	if (global.typer == 22) script_execute(149, 9, 0, self.x + 10, self.y, self.x + 200, 1, 1, 87, 11, 20);
	if (global.typer == 23) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 310, 0, 1, 95, 8, 18);
	if (global.typer == 24) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 310, 0, 1, 65, 8, 18);
	if (global.typer == 27) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 56, 8, 18);
	if (global.typer == 28) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 65, 8, 18);
	if (global.typer == 30) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, 9999, 0, 2, 90, 20, 36);
	if (global.typer == 31) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, 9999, 0, 2, 90, 12, 18);
	if (global.typer == 32) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, 9999, 0, 2, 84, 20, 36);
	if (global.typer == 33) script_execute(149, 4, 0, self.x, self.y, self.x + 190, 43, 1, 95, 9, 20);
	if (global.typer == 34) script_execute(149, 0, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 71, 16, 18);
	if (global.typer == 35) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 84, 10, 18);
	if (global.typer == 36) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 8, 85, 10, 18);
	if (global.typer == 37) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 78, 8, 18);
	if (global.typer == 38) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 6, 78, 8, 18);
	if (global.typer == 39) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 0, 1, 78, 9, 20);
	if (global.typer == 40) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 1, 2, 78, 9, 20);
	if (global.typer == 41) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 0, 1, 78, 9, 20);
	if (global.typer == 42) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 2, 4, 78, 9, 20);
	if (global.typer == 43) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 2, 4, 80, 9, 20);
	if (global.typer == 44) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 2, 5, 81, 9, 20);
	if (global.typer == 45) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 2, 7, 82, 9, 20);
	if (global.typer == 47) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 83, 8, 18);
	if (global.typer == 48) script_execute(149, 8, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 89, 8, 18);
	if (global.typer == 49) script_execute(149, 4, 16777215, self.x, self.y, self.x + 190, 43, 1, 83, 9, 20);
	if (global.typer == 50) script_execute(149, 1, 16777215, self.x + 20, self.y + 10, 999, 0, 3, 56, 8, 18);
	if (global.typer == 51) script_execute(149, 4, 0, self.x + 20, self.y + 16, 999, 0, 3, 56, 8, 18);
	if (global.typer == 52) script_execute(149, 4, 0, self.x + 20, self.y + 20, 999, 0, 1, 83, 8, 18);
	if (global.typer == 53) script_execute(149, 4, 0, self.x + 20, self.y + 10, 999, 1.5, 4, 56, 8, 18);
	if (global.typer == 54) script_execute(149, 4, 0, self.x + 20, self.y + 10, 999, 0, 7, 56, 8, 18);
	if (global.typer == 55) script_execute(149, 4, 0, self.x, self.y, self.x + 999, 0, 2, 96, 9, 20);
	if (global.typer == 60) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 90, 8, 18);
	if (global.typer == 61) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, self.x + 99999, 0, 2, 96, 16, 32);
	if (global.typer == 62) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 3, 90, 9, 20);
	if (global.typer == 63) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 2, 90, 9, 20);
	if (global.typer == 64) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 2, 3, 90, 9, 20);
	if (global.typer == 66) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 2, 97, 9, 20);
	if (global.typer == 67) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, self.x + 999, 2, 5, 98, 16, 32);
	if (global.typer == 68) script_execute(149, 4, 16777215, self.x, self.y, self.x + 500, 0, 1, 97, 9, 20);
	if (global.typer == 69) script_execute(149, 4, 16777215, self.x, self.y, self.x + 500, 2, 2, 98, 9, 20);
	if (global.typer == 70) script_execute(149, 4, 16777215, self.x, self.y, self.x + 500, 1, 3, 97, 9, 20);
	if (global.typer == 71) script_execute(149, 4, 16777215, self.x, self.y, self.x + 500, 2, 5, 98, 9, 20);
	if (global.typer == 72) script_execute(149, 4, 16777215, self.x, self.y, self.x + 500, 1, 2, 97, 9, 20);
	if (global.typer == 73) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, self.x + 99999, 0, 5, 96, 16, 32);
	if (global.typer == 74) script_execute(149, 4, 0, self.x, self.y, self.x + 490, 0, 1, 83, 9, 20);
	if (global.typer == 75) script_execute(149, 4, 0, self.x, self.y, self.x + 490, 2, 1, 83, 9, 20);
	if (global.typer == 76) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 84, 8, 18);
	if (global.typer == 77) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 4, 98, 9, 20);
	if (global.typer == 78) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 2, 3, 98, 9, 20);
	if (global.typer == 79) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 85, 8, 18);
	if (global.typer == 80) script_execute(149, 8, 0, self.x, self.y, self.x + 200, 0, 1, 88, 10, 20);
	if (global.typer == 81) script_execute(149, 4, 0, self.x, self.y, self.x + 190, 0, 1, 78, 9, 20);
	if (global.typer == 82) script_execute(149, 4, 0, self.x, self.y, self.x + 490, 2, 3, 83, 9, 20);
	if (global.typer == 83) script_execute(149, 9, 0, self.x + 2, self.y, self.x + 200, 1, 3, 87, 11, 20);
	if (global.typer == 84) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 1, 2, 99, 10, 20);
	if (global.typer == 85) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 2, 84, 9, 20);
	if (global.typer == 86) script_execute(149, 4, 0, self.x + 10, self.y, self.x + 200, 0, 1, 85, 9, 20);
	if (global.typer == 87) script_execute(149, 4, 0, self.x + 10, self.y, self.x + 200, 0, 3, 85, 9, 20);
	if (global.typer == 88) script_execute(149, 4, 0, self.x + 10, self.y, self.x + 200, 2, 3, 85, 9, 20);
	if (global.typer == 89) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 84, 8, 18);
	if (global.typer == 90) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 84, 8, 18);
	if (global.typer == 91) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, 9999, 0, 3, 101, 10, 18);
	if (global.typer == 92) script_execute(149, 4, 16777215, self.x, self.y, self.x + 190, 43, 1, 95, 9, 20);
	if (global.typer == 93) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 0, 1, 79, 9, 20);
	if (global.typer == 94) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 1, 2, 79, 9, 20);
	if (global.typer == 95) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 2, 3, 79, 9, 20);
	if (global.typer == 96) script_execute(149, 4, 0, self.x + 16, self.y, self.x + 190, 3, 4, 79, 9, 20);
	if (global.typer == 97) script_execute(149, 4, 0, self.x + 16, self.y, 999, 1, 3, 56, 8, 18);
	if (global.typer == 98) script_execute(149, 4, 0, self.x + 8, self.y, self.x + 200, 0, 1, 97, 9, 20);
	if (global.typer == 99) script_execute(149, 4, 0, self.x + 8, self.y, self.x + 200, 1, 1, 97, 9, 20);
	if (global.typer == 100) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 96, 8, 18);
	if (global.typer == 101) script_execute(149, 4, 0, self.x + 8, self.y, self.x + 200, 1, 2, 97, 9, 20);
	if (global.typer == 102) script_execute(149, 4, 0, self.x + 8, self.y, self.x + 200, 2, 3, 97, 9, 20);
	if (global.typer == 103) script_execute(149, 4, 0, self.x + 8, self.y, self.x + 200, 2, 5, 84, 9, 20);
	if (global.typer == 104) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, 999, 0, 4, 96, 16, 34);
	if (global.typer == 105) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, 999, 0, 3, 96, 16, 34);
	if (global.typer == 106) script_execute(149, 2, 16777215, self.x + 20, self.y + 20, 999, 0, 3, 96, 8, 18);
	if (global.typer == 107) script_execute(149, 8, 0, self.x + 5, self.y, self.x + 200, 0, 2, 88, 10, 20);
	if (global.typer == 108) script_execute(149, 4, 0, self.x, self.y, self.x + 200, 0, 4, 96, 9, 20);
	if (global.typer == 109) script_execute(149, 8, 0, self.x + 5, self.y, self.x + 200, 0, 1, 88, 10, 20);
	if (global.typer == 110) script_execute(149, 1, 16777215, self.x + 20, self.y + 20, 9999, 0, 2, 88, 20, 36);
	if (global.typer == 111) script_execute(149, 4, 0, self.x, self.y, self.x + 190, 43, 1, 95, 9, 20);
	if (global.typer == 666) script_execute(149, 0, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 1, 4, 71, 16, 18);
	*/
}

void OBJ_WRITER::RefreshQuads() {
	_quads.clear();
	Vector2f my = _writing;
	Vector2f start;
	Color color = setup.color;
	auto glyphs = Undertale::GetFontGlyphs(setup.myfont);
	for (int n = 0; n < _pos; n++) {
		char ch = _text[n];
		char nch = (n + 1) < _text.length() ? _text[n + 1] : 0;
		switch (ch) {
		case '&':
			my.x = _writing.x;
			my.y += setup.vspacing;
			_lineno++;
			break;
		case '\\':
			switch (nch) {
			case 'R': color = Color::Red; break;
			case 'W': color = Color::White; break;
			case 'Y': color = Color::Yellow;  break;
			case 'X': color = Color::Black; break;
			case 'B': color = Color::Blue; break;
			case 'C': break; // choise see obj_choicer
			case 'M': break; // something with flag[20], animation index?
			case 'E': if (face != nullptr) face->changeEmotion(nch - '0'); 	break;
			case 'F': if (face != nullptr) face->changeFace(nch - '0'); 	break;
			case 'T': // just for tor?  chagnes the typer
				switch (_text[n + 2]) {
				case 'T': SetTextType(4); break;
				case 't':SetTextType(48); break;
				case '0':SetTextType(5); break;
				case 'S':SetTextType(10); break;
				case 'F':SetTextType(16); break;
				case 's':SetTextType(17); break;
				case 'P':SetTextType(18); break;
				case 'M':SetTextType(27); break;
				case 'U':SetTextType(37); break;
				case 'A':SetTextType(47); break;
				case 'a':SetTextType(60); break;
				case 'R':SetTextType(76); break;
				}
				n++;
				break;
			case 'z': break; // what the hell is Z? OOOH its a shaking infinity sign for the asriel dremo fight

			}
			n++;
			break;
		case '/':
			if (nch == '%') _halt = 2;
			else if (nch == '^' && _text[n + 2] != '0') _halt = 4;
			else if (nch == '*') _halt = 6;
			else _halt = 1;
			return;
		case '%':
			if (nch == '%') return; // die here
			else {
				Reset(); // next line
				_stringno++;
				_text = _lines[_stringno++];
				_quads.clear();
				n = -1; // reset
				continue;
			}
		default:
		{
			if (my.x > setup.writing.width) {
				my.x = _writing.x;
				my.y += setup.vspacing;
				_lineno++;
			} // new line
			// text fixes
			if (_typer == 18) {
				switch (ch) {
				case 'l': case 'i': case 'I': case '!': case '.': case '?':
					my.x += 2;
					break;
				case 'S': case 'D': case 'A': case '\'':
					my.x++;
					break;
				}
			}
			auto& g = glyphs[ch];
			switch (setup.shake) {
			case 0:
				addGlyphQuad(_quads, my, color, g);
				break;
			case 39:
				//   self.direction+= 10;
					// draw_text(self.myx + self.hspeed, self.myy + self.vspeed, self.myletter);
			case 40:
				//	self.direction += 20 * self.n;
				//	draw_text(self.myx + self.hspeed, self.myy + self.vspeed, self.myletter);
				//	self.direction -= 20 * self.n;
					//start = CreateMovementVector(20, 2) + my;
				//	addGlyphQuad(_quads, start, color, g);
			case 41:

				//	self.direction += 10 * self.n;
				//	draw_text(self.myx + self.hspeed, self.myy + self.vspeed, self.myletter);
				//	self.direction -= 10 * self.n;
			case 43:
				//self.direction += 30 * self.n;
				//draw_text(self.myx + self.hspeed * 0.7 + 10, self.myy + self.vspeed * 0.7, self.myletter);
				//self.direction -= 30 * self.n;
				break;
			default:
				assert(setup.shake > 0);
				start = Vector2f((std::rand() % setup.shake) - setup.shake / 2, (std::rand() % setup.shake) - setup.shake / 2);
				addGlyphQuad(_quads, start + my, color, g);
				break;
			}
			my.x += setup.spacing;
			if (setup.myfont == 8) {
				switch (ch) {
				case 'w': case 'm': case 'i': case 'l': my.x += 2;
				case 's': case 'j': my.x -= 1;
				}
			}
			if (setup.myfont == 9) {
				if (ch == 'D') my.x += 1;
				if (ch == 'Q') my.x += 3;
				if (ch == 'M') my.x += 1;
				if (ch == 'L') my.x -= 1;
				if (ch == 'K') my.x -= 1;
				if (ch == 'C') my.x += 1;
				if (ch == '.') my.x -= 3;
				if (ch == '!') my.x -= 3;
				if (ch == 'O' || ch == 'W') my.x += 2;
				if (ch == 'I') my.x -= 6;
				if (ch == 'T') my.x -= 1;
				if (ch == 'P') my.x -= 2;
				if (ch == 'R') my.x -= 2;
				if (ch == 'A') my.x += 1;
				if (ch == 'H') my.x += 1;
				if (ch == 'B') my.x += 1;
				if (ch == 'G') my.x += 1;
				if (ch == 'F') my.x -= 1;
				if (ch == '?') my.x -= 3;
				if (ch == '\'') my.x -= 6;
				if (ch == 'J') my.x -= 1;
			}
			break;
		}
		}
		
	}
}
void  OBJ_WRITER::frame() {
	if (_halt > 0) {
		switch (_halt) {
		case 1:
			Reset();
			_pos = 0;
			_stringno++;
			_text = _lines[_stringno++];
		case 2:
			break; // instance destroy;
		case 4:
			break; // clear return instance destory/  not in a fight?
		}
	}
	else {
		if (_textpause > 0) _textpause--;
		else {
			if (_pos < _text.length()) {
				_pos++;
				_textpause = setup.textspeed;
				char ch = _text[_pos];
				char nch = (_pos + 1) < _text.length() ? _text[_pos + 1] : 0;
				if (ch == '^') {
					if (nch != '0') {
						_textpause = 10 * (nch - '0');
					}
					_pos += 2;
				}
				else {
					_textSound.stop();
					_textSound.play();
				}
				if(ch == '&') _pos++;
				if (ch == '\\') _pos += 2;
			}
		}
	}
	RefreshQuads();
}


void OBJ_WRITER::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = _texture;
	target.draw(_quads, states);
}
