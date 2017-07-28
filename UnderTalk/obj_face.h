#pragma once
#include "Global.h"
#include "room.h"

class obj_face  : public RoomObject {
	size_t _emotion;
	size_t _face;
	// special case for torel as we have multipule sprites to overlap
	float _time;
	float _last;

public:
	obj_face(Room& room) : RoomObject(room, 774) {} // obj face is 774
	~obj_face();
	size_t getFace() { return _face; }
	virtual size_t getEmotion() const { return _emotion;  }
	bool hasFace() const { return _face != 0; }
	void setEmotion(size_t e);
	void setFace(size_t c);
};
