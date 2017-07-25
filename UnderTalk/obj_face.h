#pragma once
#include "Global.h"
#include "room.h"
#include "gsprites.h"

class obj_face  : public sf::Drawable {
	ManagerType& _manager;
	entity_t _faceSprites[3];
	size_t _emotion;
	size_t _face;
	// special case for torel as we have multipule sprites to overlap
	float _time;
	float _last;

public:
	obj_face(ManagerType& manager;
	size_t getFace() { return _face; }
	virtual size_t getEmotion() const { return _emotion;  }
	bool hasFace() const { return _face != 0; }
	void setEmotion(size_t e);
	void setFace(size_t c);
};
