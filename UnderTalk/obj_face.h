#pragma once
#include "Global.h"
#include "room.h"
#include "gsprites.h"

class obj_face : RoomObject {
	size_t _emotion;
	size_t _choice;
	GSprite _face; 
	float _time;
	float _last;
public:
	obj_face(Room* room) : RoomObject(room),_choice(0), _emotion(0) , _time(0.0) { }
	GSprite& getFace() { return _face; }
	virtual size_t getEmotion() const { return _emotion;  }
	size_t getFaceChoice() const { return _choice; }
	bool hasFace() const { return _choice != 0; }
	void setEmotion(size_t e);
	void setFaceChoice(size_t c);
};
