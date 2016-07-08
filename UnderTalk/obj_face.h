#pragma once
#include "Global.h"
#include "gsprites.h"

class obj_face : public sf::Drawable {
	size_t _emotion;
	size_t _choice;
	struct gsprite_depth {
		bool operator() (const GSprite& lhs, const GSprite& rhs) const {
			return lhs.getDepth() < rhs.getDepth();
		}
		bool operator() (const GSprite* lhs, const GSprite* rhs) const {
			return lhs->getDepth() < rhs->getDepth();
		}
	};

	std::set<GSprite*, gsprite_depth> _sprites;
	GSprite _face; 
	std::vector<GSprite> _extra; // extra stuff like tor body or her glasses
	float _time;
	float _last;
public:
	obj_face() : _choice(0), _emotion(0) , _time(0.0) { }
	GSprite& getFace() { return _face; }
	virtual size_t getEmotion() const { return _emotion;  }
	size_t getFaceChoice() const { return _choice; }
	bool hasFace() const { return _choice != 0; }
	void setEmotion(size_t e);
	void setFaceChoice(size_t c);

	void step(float dt) {
		static const float _frameTime = (1.0f / (30.0f* 0.25f)); // room is 30fps and each face changes the animation time to 25 slower
		_time += dt;
		if (_time > _frameTime) {
			_time = 0.0;
			_face.setImageIndex(_face.getImageIndex() + 1);
		}
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		for (auto& s : _sprites) target.draw(*s, states);
	}
};
