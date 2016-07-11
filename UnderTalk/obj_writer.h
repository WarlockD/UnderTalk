#pragma once
#include "Global.h"
#include "gsprites.h"
#include "obj_face.h"


class OBJ_WRITER : public sf::Drawable, public sf::Transformable {
	
	mutable sf::VertexArray _quads;           ///< Vertex array containing the fill geometry
	mutable std::vector<GSprite> _sprites;
	mutable const sf::Texture* _texture;
	sf::SoundBuffer _textSoundBuffer;
	sf::Sound _textSound;

	std::string _text;
	std::vector<std::string> _lines;
	int _size;
	struct TextSetup {
		int myfont;
		int color;
		int writingx;
		int writingy;
		int writingxend;
		float shake;
		int textspeed;
		int txtsound;
		int spacing;
		int vspacing;
	};
	static std::map<size_t, OBJ_WRITER::TextSetup> setups;
	TextSetup setup;
	sf::Vector2f _writing;
	int _typer;
	size_t _stringno;
	size_t _pos;
	size_t _lineno;
	int _halt;
	int _textpause;
	// text position
public:
	obj_face face;
	OBJ_WRITER(Room* room) :  _text(""), _texture(nullptr), _quads(sf::PrimitiveType::Triangles), _size(0), _stringno(0), _pos(0), _lineno(0), _halt(0) { SetTextType(4);  }
	void AddText(const std::string& text) { _lines.push_back(text); _text = _lines[0];  }
	void Reset() {
		_pos = 0;
		_lineno = 0;
		_halt = 0;
		_writing = sf::Vector2f((float)setup.writingx, (float)setup.writingy);
		_stringno = 0;
		_textpause = setup.textspeed;
	}
	void RefreshQuads();
	void SetTextType(int type);
	virtual void frame(); // update frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

