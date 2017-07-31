#pragma once
#include "Global.h"
#include "gsprites.h"
#include "obj_face.h"
#include "room.h"

class UndertaleText {
public:
	enum class Token : int {
		Bad=0, // stop value = 0 means nothin after this
		Letter,
		Delay, // "^[0-9]" // 0 is default
		NewLine, // "&", also checks for \n|\r|\r\n|\n\r
		Color,
		Choicer,
		Face,
		Emotion,
		Typer,  // all follow a \  
		Flag, // Usally this was global.flag[20]  Used for animations and extra sprite movements
		Infinity, // its a shaking infinity sign for the asriel dremo fight when checking his stats  //z
		NextString, // "%"
		SelfDestroy, // "%%"
		Halt // all follow a /
	};
	class token_t;
	typedef std::vector<token_t> token_container;
	typedef std::vector<token_t>::iterator iterator;
	typedef std::vector<token_t>::const_iterator const_iterator;
	class token_t {
		Token _token;
		int _value;	//	value of token 
	public:
		token_t() : _token(Token::Bad), _value(0) {}
		token_t(Token token) : _token(token), _value(0) {}
		token_t(Token token, int value) : _token(token), _value(value) {}
		inline Token token() const { return _token; }
		inline int value() const { return _value; }
		bool operator==(const token_t& other) const { return _token == other._token; }
		bool operator!=(const token_t& other) const { return _token != other._token; }
		bool operator==(const Token& token) const { return _token == token; }
		bool operator!=(const Token& token) const { return _token != token; }
	};
private:
	std::string _text;
	std::vector<token_t> _parsed;
	size_t _charCount; // number of displable chars
	void parse();
public:
	UndertaleText() {}
	UndertaleText(const std::string& text) : _text(text) { parse(); }
	const std::string& getText() const { return _text; }
	void setText(const std::string& text) { _text = text; parse(); }
	const token_container& getTokens() const { return _parsed; }
	iterator begin() { return _parsed.begin(); }
	iterator end() { return _parsed.end(); }
	const_iterator begin() const { return _parsed.begin(); }
	const_iterator end() const { return _parsed.end(); }
	token_t operator[](size_t i) const { return i >= _parsed.size() ? token_t() : _parsed[i]; }
	size_t size() const { return _parsed.size(); }
	size_t charCount() const { return _charCount; }
};

class OBJ_WRITER : public RoomObject, public NodeEvent<sf::Event> {
public:
	typedef std::function<int(OBJ_WRITER&, int)> HaltDelegate;

	// ugh tired of this, lets just make a parsing system
private:
	sf::SoundBuffer _textSoundBuffer;
	sf::Sound _textSound;

	UndertaleText _tokens;
	std::vector<std::string> _lines;
	std::vector<sf::Vertex> _glyphVertices;
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
	class GlyphUpdater :  public SpriteVertices{
		float _shake;
		sf::Vector2f _home;
	public:
		GlyphUpdater(std::vector<sf::Vertex>& verteices, size_t index, float shake, const sf::IntRect& rect, const sf::Vector2f& pos, sf::Color color) : _home(pos),
			SpriteVertices(verteices.data() + index, pos, rect,color), _shake(shake) {}
		// we don't care about dt, as OBJ_WRITER caculates the frame for us
		virtual void step(float dt) ;
	};
	std::vector<GlyphUpdater> _updaters;
	static std::map<size_t, OBJ_WRITER::TextSetup> setups;
	//sf::VertexArray _glyphVertices;
	TextSetup setup;
	sf::Vector2f _writing;
	int _typer;
	size_t _stringno;
	UndertaleText::const_iterator _pos;
	int _halt;
	int _textpause;
	sf::Color _currentColor;
	// text position
//	obj_face _face;
	const std::map<int, sf::Glyph>* _fontGlyphs;
	const sf::Texture* _fontTexture;
	HaltDelegate _haltdel;
	entityplus::subscriber_handle<sf::Event::KeyEvent> handle;
public:
	void setHaltDelegate(HaltDelegate func) { _haltdel = func; }

	void DebugSetFace(int face, int emotion) { 
	//	_face.setFace(face); _face.setEmotion(emotion); 
	}
	OBJ_WRITER(Room& room) : RoomObject(room,0), _fontTexture(nullptr), _size(0), _stringno(0), _halt(0), _fontGlyphs(nullptr) , _glyphVertices(sf::PrimitiveType::TrianglesStrip){
		SetTextType(0); 
		_glyphVertices.reserve(6 * 200); // shouldn't need more than this?


	}
	bool event(const sf::Event& e) {
		if (sf::Event::KeyPressed == e.type) {
			switch (_halt) {
			case 1:
				/*
				self.myletter = " ";
				self.stringpos = 1;
				self.stringno++;
				self.originalstring = self.mystring[self.stringno];
				self.myx = self.writingx;
				self.myy = self.writingy;
				self.lineno = 0;
				self.halt = 0;
				self.alarm[0] = self.textspeed;
				*/
				return false;
			case 2:
				removeSelf();
				return false;
			case 4:
#if 0
				global.myfight = 0;
				global.mnfight = 1;
				keyboard_clear(13/* ENTER */);
				instance_destroy();
#endif
				return false;
			}
		}
		return false;
	}
	void AddText(const std::string& text) { _lines.push_back(text);  }
	void Reset() {
		_halt = 0;
		_writing = sf::Vector2f((float)setup.writingx, (float)setup.writingy);
		_glyphVertices.clear();
		_updaters.clear();
		_currentColor = sf::Color(setup.color);
		_textpause = setup.textspeed;
		if (_stringno < _lines.size())
			_tokens.setText(_lines[_stringno]);
		else
			_tokens.setText("%%");
		_pos = _tokens.begin();
	}
	void SetTextType(int type);
	void setFont(int index);

	virtual void step(float dt) override; // update frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
#if 0
class obj_dialoguer : public RoomObject, public NodeEvent<sf::Event> {
	int _count;
	int _side;
	sf::Vector2f _pos;
public:
	obj_dialoguer() {}
	void on_create() {
		//Room* room = getRoom();
		//room->instance_create(10, 150);


	}
#if 0
	self.count = 0;
	self.side = 0;
	global.facechange = 1;
	self.xx = self.view_xview[self.view_current];
	self.yy = self.view_yview[self.view_current];
	if (instance_exists(1570/* obj_mainchara */)) {
		if (obj_mainchara.y > self.yy + 130) {
			self.side = 0;
			if (global.facechoice != 0) {
				self.writer = instance_create(self.xx + 68, self.yy - 5, 782/* OBJ_WRITER */);
				script_execute(144/* scr_facechoice */);
			}
			else  self.writer = instance_create(self.xx + 10, self.yy - 5, 782/* OBJ_WRITER */);
		}
		else {
			self.side = 1;
			if (global.facechoice != 0) {
				self.writer = instance_create(self.xx + 68, self.yy + 150, 782/* OBJ_WRITER */);
				script_execute(144/* scr_facechoice */);
			}
			else  self.writer = instance_create(self.xx + 10, self.yy + 150, 782/* OBJ_WRITER */);
		}

#endif
};

#endif