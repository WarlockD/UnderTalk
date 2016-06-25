#pragma once
#include "Global.h"


namespace Undertale {

	const std::map<int,sf::Glyph>& GetFontGlyphs(int font_index);
	const sf::Texture* GetFontTexture(int font_index);
	int GetFontSize(int font_index);
}
struct obj_face {
	virtual void changeEmotion(int i) = 0;
	virtual void changeFace(int i) = 0;
	virtual ~obj_face() {}
};
class OBJ_WRITER : public sf::Drawable, public sf::Transformable {
	mutable sf::VertexArray _quads;           ///< Vertex array containing the fill geometry
	mutable std::vector<sf::Sprite> _sprites;
	mutable const sf::Texture* _texture;
	sf::SoundBuffer _textSoundBuffer;
	sf::Sound _textSound;

	std::string _text;
	std::vector<std::string> _lines;
	int _size;
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
	TextSetup setup;
	sf::Vector2f _writing;
	int _typer;
	int _stringno;
	int _pos;
	int _lineno;
	int _halt;
	int _textpause;
	// text position
public:
	obj_face* face;
	static void LoadAllFonts();
	OBJ_WRITER() : _text(""), _texture(nullptr), _quads(sf::PrimitiveType::Triangles) , _size(0), _stringno(0), _pos(0), _lineno(0), _halt(0), face(nullptr) {}
	void AddText(const std::string& text) { _lines.push_back(text); _text = _lines[0];  }
	void Reset() {
		_pos = 0;
		_lineno = 0;
		_halt = 0;
		_writing = sf::Vector2f(std::roundf(setup.writing.left), std::roundf(setup.writing.top));
		_stringno = 0;
		_textpause = setup.textspeed;
	}
	void RefreshQuads();
	void SetTextType(int type);
	virtual void frame(); // update frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

