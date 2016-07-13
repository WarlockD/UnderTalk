#pragma once
#include "Global.h"
#include "room.h"
#include "gsprites.h"

class obj_face : RoomObject {
	size_t _emotion;
	size_t _face;
	SpriteNode _faceSprite; 
	float _time;
	float _last;
public:
	obj_face() : RoomObject(), _face(0), _emotion(0), _time(0.0) {  }
	size_t getFace() { return _face; }
	virtual size_t getEmotion() const { return _emotion;  }
	bool hasFace() const { return _face != 0; }
	void setEmotion(size_t e);
	void setFace(size_t c);
};
