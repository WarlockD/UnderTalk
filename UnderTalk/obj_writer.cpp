#include "obj_writer.h"
#include "gsprites.h"


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

std::map<size_t, OBJ_WRITER::TextSetup> OBJ_WRITER::setups = {
{ 1, { 1, Color::White, FloatRect(20.0f,20.0f,  -55.0f,0.0f), 1, 1, 94, 16, 32 } },
{ 2, { 4, Color::Black, FloatRect(0.0f, 0.0f,190.0f,0.0f), 43, 2, 95, 9, 20 } },
{ 3, { 4, Color(8421376), FloatRect(0.0f, 0.0f,  100.0f,0.0f),  39, 3, 95, 10, 10 } },
{ 4, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 101, 8, 18 } }, // view.x290.0f
{ 5, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 95, 8, 18 } }, // view.x290.0f
{ 6, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 1, 97, 9, 20 } },
{ 7, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 2, 2, 98, 9, 20 } },
{ 8, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 1, 101, 9, 20 } },
{ 9, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 97, 8, 18 } }, // view.x290.0f
{ 10, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 96, 8, 18 } }, // view.x290.0f
{ 11, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 2, 94, 9, 18 } }, // view.x290.0f
{ 12, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 1, 3, 99, 10, 20 } },
{ 13, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 2, 4, 99, 11, 20 } },
{ 14, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 3, 5, 99, 14, 20 } },
{ 15, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 10, 99, 18, 20 } },
{ 16, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 1.2, 2, 98, 8, 18 } }, // view.x290.0f
{ 17, { 8, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 88, 8, 18 } }, // view.x290.0f
{ 18, { 9, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 87, 11, 18 } }, // view.x290.0f
{ 19, { 9, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 87, 11, 18 } }, // view.x290.0f //	if (global.typer == 19) global.typer = 18;
{ 20, { 5, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 2, 98, 25, 20 } },
{ 21, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 4, 96, 10, 18 } }, // view.x290.0f
{ 22, { 9, Color::Black, FloatRect(10.0f, 0.0f,200.0f,0.0f), 1, 1, 87, 11, 20 } },
{ 23, { 2, Color::White, FloatRect(20.0f,20.0f,310.0f,0.0f), 0, 1, 95, 8, 18 } }, // view.x310.0f
{ 24, { 2, Color::White, FloatRect(20.0f,20.0f,310.0f,0.0f), 0, 1, 65, 8, 18 } }, // view.x310.0f
{ 27, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 3, 56, 8, 18 } }, // view.x290.0f
{ 28, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 2, 65, 8, 18 } }, // view.x290.0f
{ 20, { 1, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 0, 2, 90, 20, 36 } },
{ 31, { 2, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 0, 2, 90, 12, 18 } },
{ 32, { 1, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 0, 2, 84, 20, 36 } },
{ 33, { 4, Color::Black, FloatRect(0.0f, 0.0f,190.0f,0.0f), 43, 1, 95, 9, 20 } },
{ 34, { 0, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 3, 71, 16, 18 } }, // view.x290.0f
{ 35, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 2, 84, 10, 18 } }, // view.x290.0f
{ 36, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 8, 85, 10, 18 } }, // view.x290.0f
{ 37, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 78, 8, 18 } }, // view.x290.0f
{ 38, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 6, 78, 8, 18 } }, // view.x290.0f
{ 39, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 0, 1, 78, 9, 20 } },
{ 40, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 1, 2, 78, 9, 20 } },
{ 41, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 0, 1, 78, 9, 20 } },
{ 42, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 2, 4, 78, 9, 20 } },
{ 43, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 2, 4, 80, 9, 20 } },
{ 44, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 2, 5, 81, 9, 20 } },
{ 45, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 2, 7, 82, 9, 20 } },
{ 47, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 83, 8, 18 } }, // view.x290.0f
{ 48, { 8, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 89, 8, 18 } }, // view.x290.0f
{ 49, { 4, Color::White, FloatRect(0.0f, 0.0f,190.0f,0.0f), 43, 1, 83, 9, 20 } },
{ 50, { 1, Color::White, FloatRect(20.0f,10.0f,99.0f,0.0f), 0, 3, 56, 8, 18 } },
{ 51, { 4, Color::Black, FloatRect(20.0f,16.0f,99.0f,0.0f), 0, 3, 56, 8, 18 } },
{ 52, { 4, Color::Black, FloatRect(20.0f,20.0f,99.0f,0.0f), 0, 1, 83, 8, 18 } },
{ 53, { 4, Color::Black, FloatRect(20.0f,10.0f,99.0f,0.0f), 1.5, 4, 56, 8, 18 } },
{ 54, { 4, Color::Black, FloatRect(20.0f,10.0f,99.0f,0.0f), 0, 7, 56, 8, 18 } },
{ 55, { 4, Color::Black, FloatRect(0.0f, 0.0f,999.0f,0.0f), 0, 2, 96, 9, 20 } },
{ 60, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 2, 90, 8, 18 } }, // view.x290.0f
{ 61, { 1, Color::White, FloatRect(20.0f,20.0f,99999.0f,0.0f), 0, 2, 96, 16, 32 } },
{ 62, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 3, 90, 9, 20 } },
{ 63, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 2, 90, 9, 20 } },
{ 64, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 2, 3, 90, 9, 20 } },
{ 66, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 2, 97, 9, 20 } },
{ 67, { 1, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 2, 5, 98, 16, 32 } },
{ 68, { 4, Color::White, FloatRect(0.0f, 0.0f,500.0f,0.0f), 0, 1, 97, 9, 20 } },
{ 69, { 4, Color::White, FloatRect(0.0f, 0.0f,500.0f,0.0f), 2, 2, 98, 9, 20 } },
{ 70, { 4, Color::White, FloatRect(0.0f, 0.0f,500.0f,0.0f), 1, 3, 97, 9, 20 } },
{ 71, { 4, Color::White, FloatRect(0.0f, 0.0f,500.0f,0.0f), 2, 5, 98, 9, 20 } },
{ 72, { 4, Color::White, FloatRect(0.0f, 0.0f,500.0f,0.0f), 1, 2, 97, 9, 20 } },
{ 73, { 1, Color::White, FloatRect(20.0f,20.0f,99999.0f,0.0f), 0, 5, 96, 16, 32 } },
{ 74, { 4, Color::Black, FloatRect(0.0f, 0.0f,490.0f,0.0f), 0, 1, 83, 9, 20 } },
{ 75, { 4, Color::Black, FloatRect(0.0f, 0.0f,490.0f,0.0f), 2, 1, 83, 9, 20 } },
{ 76, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 84, 8, 18 } }, // view.x290.0f
{ 77, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 4, 98, 9, 20 } },
{ 78, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 2, 3, 98, 9, 20 } },
{ 79, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 2, 85, 8, 18 } }, // view.x290.0f
{ 80, { 8, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 1, 88, 10, 20 } },
{ 81, { 4, Color::Black, FloatRect(0.0f, 0.0f,190.0f,0.0f), 0, 1, 78, 9, 20 } },
{ 82, { 4, Color::Black, FloatRect(0.0f, 0.0f,490.0f,0.0f), 2, 3, 83, 9, 20 } },
{ 83, { 9, Color::Black, FloatRect(2.0f, 0.0f,200.0f,0.0f), 1, 3, 87, 11, 20 } },
{ 84, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 1, 2, 99, 10, 20 } },
{ 85, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 2, 84, 9, 20 } },
{ 86, { 4, Color::Black, FloatRect(10.0f, 0.0f,200.0f,0.0f), 0, 1, 85, 9, 20 } },
{ 87, { 4, Color::Black, FloatRect(10.0f, 0.0f,200.0f,0.0f), 0, 3, 85, 9, 20 } },
{ 88, { 4, Color::Black, FloatRect(10.0f, 0.0f,200.0f,0.0f), 2, 3, 85, 9, 20 } },
{ 89, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 84, 8, 18 } }, // view.x290.0f
{ 90, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 3, 84, 8, 18 } }, // view.x290.0f
{ 91, { 2, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 0, 3, 101, 10, 18 } },
{ 92, { 4, Color::White, FloatRect(0.0f, 0.0f,190.0f,0.0f), 43, 1, 95, 9, 20 } },
{ 93, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 0, 1, 79, 9, 20 } },
{ 94, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 1, 2, 79, 9, 20 } },
{ 95, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 2, 3, 79, 9, 20 } },
{ 96, { 4, Color::Black, FloatRect(16.0f, 0.0f,190.0f,0.0f), 3, 4, 79, 9, 20 } },
{ 97, { 4, Color::Black, FloatRect(16.0f, 0.0f,99.0f,0.0f), 1, 3, 56, 8, 18 } },
{ 98, { 4, Color::Black, FloatRect(8.0f, 0.0f,200.0f,0.0f), 0, 1, 97, 9, 20 } },
{ 99, { 4, Color::Black, FloatRect(8.0f, 0.0f,200.0f,0.0f), 1, 1, 97, 9, 20 } },
{ 100, { 2, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 0, 1, 96, 8, 18 } }, // view.x290.0f
{ 101, { 4, Color::Black, FloatRect(8.0f, 0.0f,200.0f,0.0f), 1, 2, 97, 9, 20 } },
{ 102, { 4, Color::Black, FloatRect(8.0f, 0.0f,200.0f,0.0f), 2, 3, 97, 9, 20 } },
{ 103, { 4, Color::Black, FloatRect(8.0f, 0.0f,200.0f,0.0f), 2, 5, 84, 9, 20 } },
{ 104, { 1, Color::White, FloatRect(20.0f,20.0f,99.0f,0.0f), 0, 4, 96, 16, 34 } },
{ 105, { 1, Color::White, FloatRect(20.0f,20.0f,99.0f,0.0f), 0, 3, 96, 16, 34 } },
{ 106, { 2, Color::White, FloatRect(20.0f,20.0f,99.0f,0.0f), 0, 3, 96, 8, 18 } },
{ 107, { 8, Color::Black, FloatRect(5.0f, 0.0f,200.0f,0.0f), 0, 2, 88, 10, 20 } },
{ 108, { 4, Color::Black, FloatRect(0.0f, 0.0f,200.0f,0.0f), 0, 4, 96, 9, 20 } },
{ 109, { 8, Color::Black, FloatRect(5.0f, 0.0f,200.0f,0.0f), 0, 1, 88, 10, 20 } },
{ 110, { 1, Color::White, FloatRect(20.0f,20.0f,999.0f,0.0f), 0, 2, 88, 20, 36 } },
{ 111, { 4, Color::Black, FloatRect(0.0f, 0.0f,190.0f,0.0f), 43, 1, 95, 9, 20 } },
{ 116, { 0, Color::White, FloatRect(20.0f,20.0f,290.0f,0.0f), 1, 4, 71, 16, 18 } }// view.x290.0f
};

void  OBJ_WRITER::SetTextType(int type) {
	auto pos = getPosition();
	setup = { 1, Color::White, FloatRect(pos.x + 20, pos.y + 20, 290,0.0f), 1,1,94,16,32 };
	Reset();
	if (!_textSoundBuffer.loadFromFile(Undertale::LookupSound(setup.txtsound))) {
		printf("Could not load sound");
	}
	else {
		_textSound.setBuffer(_textSoundBuffer);
		_textSound.setVolume(10);
	}
	
	
}

void OBJ_WRITER::RefreshQuads() {
	_quads.clear();
	_sprites.clear();
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
			case 'E': n++; face.setEmotion(_text[n + 1] - '0'); 	break;
			case 'F': 
			{
				n++;
				int fc = _text[n + 1] - '0';
				face.setFaceChoice(fc);
			}
				
				break;
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
			case 'z':
			{
				GSprite inf_sprite(837,0.0f);
				Vector2f shake = Vector2f((std::rand() % setup.shake) - setup.shake / 2, (std::rand() % setup.shake) - setup.shake / 2) + Vector2f(0, 10);
				inf_sprite.setPosition(my+shake);
				inf_sprite.setScale(2, 2);
				_sprites.emplace_back(inf_sprite);
			}
			break;
			}
			n++;
			break; // what the hell is Z? OOOH its a shaking infinity sign for the asriel dremo fight
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
	states.texture = Undertale::GetFontTexture(6);
	target.draw(_quads, states);
	if (_sprites.size() > 0) for(auto& s :_sprites) target.draw(s, states);
	if (face.hasFace()) {
		target.draw(face, states);
	}
}
