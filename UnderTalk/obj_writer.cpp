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
	inline int GetCharOrDefault(const std::string& text, size_t index) { return index < text.length() ? text[index] : 0; }
	inline int GetCharOrAssert(const std::string& text, size_t index) { 
		assert(index < text.length());
		return text[index];
	}
}





void UndertaleText::parse() {
	_charCount = 0;
	_parsed.clear();
	size_t n;
	for (size_t n = 0; n < _text.length(); n++) {
		int ch = _text[n];
		int nch = GetCharOrDefault(_text, n + 1);
		switch (ch) {
		case '&': _parsed.emplace_back(Token::NewLine); break;
		case '\r':
		case '\n':
			_parsed.emplace_back(Token::NewLine);
			if (nch != ch && (nch == '\r' || nch == '\n')) n++; // skip it
			break;
		case '^':  // delay, '0' is considered default
			_parsed.emplace_back(Token::Delay, 10 * (nch - '0'));  break;
		case '\\':
			switch (nch) {
			case 'R': _parsed.emplace_back(Token::Color, 255);  break;
			case 'G': _parsed.emplace_back(Token::Color, 65280);  break;
			case 'W': _parsed.emplace_back(Token::Color, 16777215);  break;
			case 'Y': _parsed.emplace_back(Token::Color, 65535); break;
			case 'X': _parsed.emplace_back(Token::Color, 0);  break;
			case 'B': _parsed.emplace_back(Token::Color, 16711680);  break;
			case 'O': _parsed.emplace_back(Token::Color, 4235519);  break;
			case 'L': _parsed.emplace_back(Token::Color, 16754964);  break;
			case 'P': _parsed.emplace_back(Token::Color, 16711935);  break;
			case 'C': _parsed.emplace_back(Token::Choicer); break; // choise see obj_choicer
			case 'M': _parsed.emplace_back(Token::Flag, GetCharOrAssert(_text, n + 2) - '0'); n++; break; // something with flag[20], animation index?
			case 'E': _parsed.emplace_back(Token::Emotion, GetCharOrAssert(_text, n + 2) - '0'); n++;	break;
			case 'F': _parsed.emplace_back(Token::Face, GetCharOrAssert(_text, n + 2) - '0'); n++;	break;
			case 'T': _parsed.emplace_back(Token::Typer, GetCharOrAssert(_text, n + 2)); n++;  break;
			case 'z': _parsed.emplace_back(Token::Infinity);   break;// what the hell is Z? OOOH its a shaking infinity sign for the asriel dremo fight
			default:
				// error
				assert(false);
				break;
			}
			n++; // skip nch
			break;
		case '/':
			// all halts
			if (nch == '%') _parsed.emplace_back(Token::Halt, 2); 
			else if (nch == '^' &&  GetCharOrAssert(_text,n + 2) != '0') _parsed.emplace_back(Token::Halt, 4);
			else if (nch == '*') _parsed.emplace_back(Token::Halt, 6);
			else _parsed.emplace_back(Token::Halt, 1);
			return; // done
		case '%':
			if (nch == '%') _parsed.emplace_back(Token::SelfDestroy);
			else _parsed.emplace_back(Token::NextString);
			return; // die here
		default:
			_charCount++;
			_parsed.emplace_back(Token::Letter,ch);
			break;
		}
	}
}

/*

struct TextSetup {
int myfont;
int color;
int writingx;
int writingy;
int writingxend;
int shake;
int textspeed;
int txtsound;
int spacing;
int vspacing;
};
*/

std::map<size_t, OBJ_WRITER::TextSetup> OBJ_WRITER::setups = {
{ 2,{ 4, 0,0,0, 190,  43, 2, 95, 9, 20 } },

// 1) script_execute(149/* SCR_TEXTSETUP */, 1, 16777215, self.x + 20, self.y + 20, self.x + global.idealborder[1] - 55, 1, 1, 94, 16, 32);
{ 2, { 4, 0,0,0, 190,  43, 2, 95, 9, 20 }},
{ 3, { 7, 8421376, 100 , 39, 3, 95, 10, 10  }},
// 4) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 101, 8, 18);
// 5) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 95, 8, 18);
{ 6, { 4, 0,0,0, 200 , 0, 1, 97, 9, 20 }},
{ 7, { 4, 0,0,0, 200 , 2, 2, 98, 9, 20 }},
{ 8, { 4, 0,0,0, 200 , 0, 1, 101, 9, 20 }},
// 9) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 97, 8, 18);
// 10) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 96, 8, 18);
// 11) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 94, 9, 18);
{ 12, { 4, 0,0,0, 200 , 1, 3, 99, 10, 20 }},
{ 13, { 4, 0,0,0, 200 , 2, 4, 99, 11, 20 }},
{ 14, { 4, 0,0,0, 200 , 3, 5, 99, 14, 20 }},
{ 15, { 4, 0,0,0, 200 , 0, 10, 99, 18, 20 }},
// 16) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 1.2, 2, 98, 8, 18);
// 17) script_execute(149/* SCR_TEXTSETUP */, 8, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 88, 8, 18);
// 19) global.typer = 18;
// 18) script_execute(149/* SCR_TEXTSETUP */, 9, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 87, 11, 18);
{ 20, { 5, 0,0,0, 200 , 0, 2, 98, 25, 20 }},
// 21) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 4, 96, 10, 18);
{ 22, { 9, 0, 10, 0, 200 , 1, 1, 87, 11, 20 }}, 
// 23) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 310, 0, 1, 95, 8, 18);
// 24) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 310, 0, 1, 65, 8, 18);
// 27) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 56, 8, 18);
// 28) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 65, 8, 18);
{ 30, { 1, 16777215, 20, 20, 9999 , 0, 2, 90, 20, 36 }}, 
{ 31, { 2, 16777215, 20, 20, 9999 , 0, 2, 90, 12, 18 }}, 
{ 32, { 1, 16777215, 20, 20, 9999 , 0, 2, 84, 20, 36 }}, 
{ 33, { 4, 0,0,0, 190 , 43, 1, 95, 9, 20 }},
// 34) script_execute(149/* SCR_TEXTSETUP */, 0, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 71, 16, 18);
// 35) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 84, 10, 18);
// 36) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 8, 85, 10, 18);
// 37) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 78, 8, 18);
// 38) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 6, 78, 8, 18);
{ 39, { 4, 0, 16, 0, 190 , 0, 1, 78, 9, 20 }}, 
{ 40, { 4, 0, 16, 0, 190 , 1, 2, 78, 9, 20 }}, 
{ 41, { 4, 0, 16, 0, 190 , 0, 1, 78, 9, 20 }}, 
{ 42, { 4, 0, 16, 0, 190 , 2, 4, 78, 9, 20 }}, 
{ 43, { 4, 0, 16, 0, 190 , 2, 4, 80, 9, 20 }}, 
{ 44, { 4, 0, 16, 0, 190 , 2, 5, 81, 9, 20 }}, 
{ 45, { 4, 0, 16, 0, 190 , 2, 7, 82, 9, 20 }}, 
// 47) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 83, 8, 18);
// 48) script_execute(149/* SCR_TEXTSETUP */, 8, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 89, 8, 18);
{ 49, { 4, 16777215,0,0, 190 , 43, 1, 83, 9, 20 }},
{ 50, { 1, 16777215, 20, 10, 999 , 0, 3, 56, 8, 18 }}, 
{ 51, { 4, 0, 20, 16, 999 , 0, 3, 56, 8, 18 }}, 
{ 52, { 4, 0, 20, 20, 999 , 0, 1, 83, 8, 18 }}, 
{ 53, { 4, 0, 20, 10, 999 , 1.5f, 4, 56, 8, 18 }}, 
{ 54, { 4, 0, 20, 10, 999 , 0, 7, 56, 8, 18 }}, 
{ 55, { 4, 0,0,0, 999 , 0, 2, 96, 9, 20 }},
// 60) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 90, 8, 18);
{ 61, { 1, 16777215, 20, 20, 99999 , 0, 2, 96, 16, 32 }},
{ 62, { 4, 0,0,0, 200 , 0, 3, 90, 9, 20 }},
{ 63, { 4, 0,0,0, 200 , 0, 2, 90, 9, 20 }},
{ 64, { 4, 0,0,0, 200 , 2, 3, 90, 9, 20 }},
{ 66, { 4, 0,0,0, 200 , 0, 2, 97, 9, 20 }},
{ 67, { 1, 16777215, 20, 20, 999 , 2, 5, 98, 16, 32 }},
{ 68, { 4, 16777215,0,0, 500 , 0, 1, 97, 9, 20 }},
{ 69, { 4, 16777215,0,0, 500 , 2, 2, 98, 9, 20 }},
{ 70, { 4, 16777215,0,0, 500 , 1, 3, 97, 9, 20 }},
{ 71, { 4, 16777215,0,0, 500 , 2, 5, 98, 9, 20 }},
{ 72, { 4, 16777215,0,0, 500 , 1, 2, 97, 9, 20 }},
{ 73, { 1, 16777215, 20, 20, 99999 , 0, 5, 96, 16, 32 }},
{ 74, { 4, 0,0,0, 490 , 0, 1, 83, 9, 20 }},
{ 75, { 4, 0,0,0, 490 , 2, 1, 83, 9, 20 }},
// 76) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 84, 8, 18);
{ 77, { 4, 0,0,0, 200 , 0, 4, 98, 9, 20 }},
{ 78, { 4, 0,0,0, 200 , 2, 3, 98, 9, 20 }},
// 79) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 2, 85, 8, 18);
{ 80, { 8, 0,0,0, 200 , 0, 1, 88, 10, 20 }},
{ 81, { 4, 0,0,0, 190 , 0, 1, 78, 9, 20 }},
{ 82, { 4, 0,0,0, 490 , 2, 3, 83, 9, 20 }},
{ 83, { 9, 0, 2, 0, 200 , 1, 3, 87, 11, 20 }}, 
{ 84, { 4, 0,0,0, 200 , 1, 2, 99, 10, 20 }},
{ 85, { 4, 0,0,0, 200 , 0, 2, 84, 9, 20 }},
{ 86, { 4, 0, 10, 0, 200 , 0, 1, 85, 9, 20 }}, 
{ 87, { 4, 0, 10, 0, 200 , 0, 3, 85, 9, 20 }}, 
{ 88, { 4, 0, 10, 0, 200 , 2, 3, 85, 9, 20 }}, 
// 89) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 84, 8, 18);
// 90) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 3, 84, 8, 18);
{ 91, { 2, 16777215, 20, 20, 9999 , 0, 3, 101, 10, 18 }}, 
{ 92, { 4, 16777215,0,0, 190 , 43, 1, 95, 9, 20 }},
{ 93, { 4, 0, 16, 0, 190 , 0, 1, 79, 9, 20 }}, 
{ 94, { 4, 0, 16, 0, 190 , 1, 2, 79, 9, 20 }}, 
{ 95, { 4, 0, 16, 0, 190 , 2, 3, 79, 9, 20 }}, 
{ 96, { 4, 0, 16, 0, 190 , 3, 4, 79, 9, 20 }}, 
{ 97, { 4, 0, 16, 0, 999 , 1, 3, 56, 8, 18 }}, 
{ 98, { 4, 0, 8, 0, 200 , 0, 1, 97, 9, 20 }}, 
{ 99, { 4, 0, 8, 0, 200 , 1, 1, 97, 9, 20 }}, 
// 100) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 96, 8, 18);
{ 101, { 4, 0, 8, 0, 200 , 1, 2, 97, 9, 20 }}, 
{ 102, { 4, 0, 8, 0, 200 , 2, 3, 97, 9, 20 }}, 
{ 103, { 4, 0, 8, 0, 200 , 2, 5, 84, 9, 20 }}, 
{ 104, { 1, 16777215, 20, 20, 999 , 0, 4, 96, 16, 34 }}, 
{ 105, { 1, 16777215, 20, 20, 999 , 0, 3, 96, 16, 34 }}, 
{ 106, { 2, 16777215, 20, 20, 999 , 0, 3, 96, 8, 18 }}, 
{ 107, { 8, 0, 5, 0, 200 , 0, 2, 88, 10, 20 }}, 
{ 108, { 4, 0,0,0, 200 , 0, 4, 96, 9, 20 }},
{ 109, { 8, 0, 5, 0, 200 , 0, 1, 88, 10, 20 }}, 
{ 110, { 1, 16777215, 20, 20, 9999 , 0, 2, 88, 20, 36 }}, 
{ 111, { 4, 0,0,0, 190 , 43, 1, 95, 9, 20 }}
// 666) script_execute(149/* SCR_TEXTSETUP */, 0, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 1, 4, 71, 16, 18);

	};

void  OBJ_WRITER::SetTextType(int type) {
	if (_typer == type) return;
	if (type == 0) type = 4;
	_typer = type;
	auto pos = getPosition();
//	setup = { 1, Color::White, FloatRect(pos.x + 20, pos.y + 20, 290,0.0f), 1,1,94,16,32 };
	
	switch (type) {
	case 4: 
		//script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 101, 8, 18);
	//	setup = { 7, 8421376, 20 , 20, /* view.x */ + 290, 0, 1, 101, 8, 18 };
		setup = { 2, 16777215, 20 , 20, /* view.x */ +290, 0, 1, 101, 8, 18 };
		// 4) script_execute(149/* SCR_TEXTSETUP */, 2, 16777215, self.x + 20, self.y + 20, self.view_xview[self.view_current] + 290, 0, 1, 101, 8, 18);
		break;
	default:
		setup = setups[type];
		break;
	};
	if (!_textSoundBuffer.loadFromFile(Undertale::LookupSound(setup.txtsound))) {
		printf("Could not load sound");
		setup.txtsound = -1;
	}
	else {
		_textSound.setBuffer(_textSoundBuffer);
		_textSound.setVolume(10);
	}
	setup.shake = 1;
	Reset();
	setFont(setup.myfont);
}
void OBJ_WRITER::GlyphUpdater::step(float dt)  {
	switch ((int)_shake) {
	case 0:
		break; // no shaking
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
		assert(_shake > 0);
		Vector2f shakeAmount(((float)util::random(_shake)) - (_shake / 2.0f), ((float)util::random(_shake)) - (_shake / 2.0f));
		setVertexPosition(shakeAmount + _home);
		break;
	}
}
void OBJ_WRITER::setFont(int index) {
	_fontGlyphs = &Undertale::GetFontGlyphs(index);
	_fontTexture = Undertale::GetFontTexture(6);
}
void OBJ_WRITER::step(float dt) {

	if (_halt > 0) {
		if (_haltdel) _halt = _haltdel(*this, _halt);
	}
	else {

		if (_textpause > 0) _textpause--;
		else {
			_textpause = setup.textspeed;
			while (_pos != _tokens.end()) {
				switch (_pos->token()) {
				case UndertaleText::Token::NewLine:
					_writing.x = (float)setup.writingx;
					_writing.y += setup.vspacing;
					break;
				case UndertaleText::Token::Color:
					_currentColor = Color(_pos->value());
					break;
				case UndertaleText::Token::Emotion:
					_face.setEmotion(_pos->value());
					break;
				case UndertaleText::Token::Face:
					_face.setFace(_pos->value());
					break;
				case UndertaleText::Token::Typer:
					switch (_pos->value()) {
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
					break;
				case UndertaleText::Token::Halt:
					_halt = _pos->value();
					return;
				case UndertaleText::Token::NextString:
					_stringno++;
					Reset();
					continue; // have to run the new string
				case UndertaleText::Token::Letter:
					int ch = _pos->value();
					if (_writing.x > (float)setup.writingxend) {
						_writing.x = (float)setup.writingx;
						_writing.y += (float)setup.vspacing;
					} // new line because we are at bounds
					  // text fixes
					if (_typer == 18) {
						switch (ch) {
						case 'l': case 'i': case 'I': case '!': case '.': case '?':
							_writing.x += 2.0f;
							break;
						case 'S': case 'D': case 'A': case '\'':
							_writing.x += 1.0f;
							break;
						}
					}
					// adding glyph part
					{
						auto& g = _fontGlyphs->at(ch);
						sf::Vertex* ptr = _glyphVertices.data();
						_glyphVertices.resize(_glyphVertices.size() + 6);
						GlyphUpdater glyph(_glyphVertices, _glyphVertices.size() - 6, setup.shake, g.textureRect, _writing, _currentColor);
						_updaters.emplace_back(glyph);
					}


					_writing.x += (float)setup.spacing;
					if (setup.myfont == 8) {
						switch (ch) {
						case 'w': case 'm': case 'i': case 'l': _writing.x += 2.0f;
						case 's': case 'j': _writing.x -= 1.0f;
						}
					}
					// kerning for the parpaus font
					if (setup.myfont == 9) {
						if (ch == 'D') _writing.x += 1;
						if (ch == 'Q') _writing.x += 3;
						if (ch == 'M') _writing.x += 1;
						if (ch == 'L') _writing.x -= 1;
						if (ch == 'K') _writing.x -= 1;
						if (ch == 'C') _writing.x += 1;
						if (ch == '.') _writing.x -= 3;
						if (ch == '!') _writing.x -= 3;
						if (ch == 'O' || ch == 'W') _writing.x += 2;
						if (ch == 'I') _writing.x -= 6;
						if (ch == 'T') _writing.x -= 1;
						if (ch == 'P') _writing.x -= 2;
						if (ch == 'R') _writing.x -= 2;
						if (ch == 'A') _writing.x += 1;
						if (ch == 'H') _writing.x += 1;
						if (ch == 'B') _writing.x += 1;
						if (ch == 'G') _writing.x += 1;
						if (ch == 'F') _writing.x -= 1;
						if (ch == '?') _writing.x -= 3;
						if (ch == '\'') _writing.x -= 6;
						if (ch == 'J') _writing.x -= 1;
					}
					if (setup.txtsound >= 0) {// do sound!
						_textSound.stop();
						_textSound.play();
					}
					_pos++; // ugh
					return; 
				}
				_pos++;
			}
		}
	}
	RoomObject::step(dt); // update all objects
}

void OBJ_WRITER::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	states.texture = _fontTexture;
	target.draw(_glyphVertices.data(), _glyphVertices.size(), PrimitiveType::Triangles, states);
	Node::draw(target, states);

}
