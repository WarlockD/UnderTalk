#include "obj_face.h"

using namespace sf;


void obj_face::setEmotion(size_t e) { 
	if (e != _emotion) {
		_emotion = e;
		if (_face == 0) return; // done
		switch (_face) {
		case 1: // obj_face_torieltalk
			switch (_emotion) {
			case 0: setUndertaleSprite(1986); break;
			case 1: setUndertaleSprite(2004); break;
			case 2: setUndertaleSprite(1990); break;
			case 3: setUndertaleSprite(1999); break;
			case 4: setUndertaleSprite(2000); break;
				// case 5: missing? humm
			case 6: setUndertaleSprite(1991); break;
			case 7: setUndertaleSprite(1993); break;
			case 8: setUndertaleSprite(1996); break;
			case 9: setUndertaleSprite(1987); break;
				break;
			}
			break;
		case 2: // obj_face_floweytalk
			switch (_emotion) {
			case 0: setUndertaleSprite(166); break;
			case 1: setUndertaleSprite(170); break;
			case 2: setUndertaleSprite(171); break;
			case 3: setUndertaleSprite(172); break;
			case 4: setUndertaleSprite(179); break;
			case 5: setUndertaleSprite(175); break;
			case 6: setUndertaleSprite(176); break;
			case 7: setUndertaleSprite(177); break;
			case 8: setUndertaleSprite(167); break;
		//	case 9:  break;
			}
			break;
		case 3: // obj_face_sans
			switch (_emotion) {
			case 0: setUndertaleSprite(2018); break;
			case 1: setUndertaleSprite(2022); break;
			case 2: setUndertaleSprite(2019); break;
			case 3: setUndertaleSprite(2020); break;
			case 4: setUndertaleSprite(2021); break;
			}
			break;
		case 4: //obj_face_papyrus
			switch (_emotion) {
			case 0: setUndertaleSprite(2008); break;
			case 1: setUndertaleSprite(2010); break;
			case 2: setUndertaleSprite(2012); break;
			case 3: setUndertaleSprite(2013); break;
			case 4: setUndertaleSprite(2011); break;
			case 5: setUndertaleSprite(2009); break;
			case 6: setUndertaleSprite(2014); break;
			case 7: setUndertaleSprite(2015); break;
			case 8: setUndertaleSprite(2016); break;
			case 9: setUndertaleSprite(2017); break;
			}
			break;
		case 5: // obj_face_undyne
		{

	
			static const int obj_face_undyne_faces[]{ 2025, 2026, 2029, 2031, 2032, 2033, 2034, 2036, 2039, 2040 };
			setUndertaleSprite(obj_face_undyne_faces[_emotion]);
		}
			/* // this happens per step so might have to do something here
			if (global.faceemotion == 1 && global.flag[390] == 2)
				self.sprite_index = 2027// spr_face_undyne1_3 ;
			if (global.faceemotion == 2 && global.flag[20] == 9)
				self.sprite_index = 2030// spr_face_undyne2_2 ;
			if (global.faceemotion == 9 && global.flag[390] == 1)
				self.sprite_index = 2041// spr_face_undyne9_2 ;
			if (global.faceemotion == 9 && global.flag[390] == 2)
				self.sprite_index = 2042// spr_face_undyne9_3 ;
			*/
			break;
		case 6: // obj_face_alphys
		{
			static const int spr_alphysface_faces[]{ 2046, 2049, 2050, 2051, 2052, 2053, 2054, 2055, 2056, 2057, 2058, 2059, 2060, 2061, 2062, 2063, 2064, 2065, 2066, 2067, 2068, 2069, 2070, 2071, 2072, 2073 };
			setUndertaleSprite(spr_alphysface_faces[_emotion]);
		}
			// shit tone here, depends on the global flag 430
		/*
	
			if (global.flag[430] == 1)
				self.sprite_index = self.u[global.faceemotion + 10];
			if (global.flag[430] == 2)
				self.sprite_index = self.u[global.faceemotion + 15];
			if (global.flag[430] == 4) {
				self.image_index = global.faceemotion;
				self.sprite_index = 2045// spr_alphysface_new 
			}
			*/
			// then the step?
			/*
			 if(global.flag[430] == 0) self.sprite_index= self.u[global.faceemotion];
			 if(instance_exists(782// OBJ_WRITER )) {
			 if (OBJ_WRITER.halt != 0) {
				 self.image_speed = 0;
				 self.image_index = 0;
			 }
			 else  self.image_speed = 0.25;
	}
	if (global.flag[430] == 1)
		self.sprite_index = self.u[global.faceemotion + 10];
	if (global.flag[430] == 2)
		self.sprite_index = self.u[global.faceemotion + 15];
	if (global.flag[430] == 3) {
		self.sprite_index = self.u[global.faceemotion];
		if (global.faceemotion == 7) self.sprite_index = self.u[24];
		if (global.faceemotion == 0) self.sprite_index = self.u[25];
	}
	if (global.flag[7] == 1) {
		self.sprite_index = self.u[global.faceemotion];
		if (global.faceemotion == 1) self.sprite_index = 2047// spr_alphysface_laugh ;
		if (global.faceemotion == 2) self.sprite_index = 2048// spr_alphysface_smarmy ;
		if (global.faceemotion == 8) self.sprite_index = 2050// spr_alphysface_2 ;
		if (global.faceemotion == 9) self.sprite_index = self.u[24];
	}
	if (global.flag[430] == 4) {
		self.image_index = global.faceemotion;
		self.sprite_index = 2045// spr_alphysface_new ;
	}
	*/
			break;
		case 7: // obj_face_asgore
			// atleast this guy is simple
		{
			static const int spr_alphysface_faces[]={ 2075, 2076, 2077, 2078, 2079, 2080 };
			setUndertaleSprite(spr_alphysface_faces[_emotion]);
		}
				break;
		case 8: // obj_face_mettaton
			setUndertaleSprite(_emotion); // this cannot be right, can it? 
			break;
		case 9: // obj_face_asriel
		{
			static const int spr_face_asriel_faces[] = { 2082, 2083, 2084, 2085, 2086, 2087, 2088, 2089, 2090, 2091 };
			setUndertaleSprite(spr_face_asriel_faces[_emotion]);
		}
			break;
		}
	}
}

void obj_face::setFace(size_t c) { 
	if (c != _face) {
		removeAllChildren();
		_face = c;
		if (c == 0) return; // done
		setDepth(0);
		sf::Vector2f pos;
		switch (_face) {
		case 1: // obj_face_torieltalk
		{
			pos = Vector2f(-33.0f, 25.0f);
			SpriteNode* tor_body = new SpriteNode();
			tor_body->setUndertaleSprite(1985);
			tor_body->setTag(1985); // body
			tor_body->setDepth(-100); // under
			addChild(tor_body);
			
		//	obj->setPosition()
		//	instance_create(getPosition().x, 0.0f, 785, 1985); 
		//	Vector2f center(Vector2f(getPosition().x, 0.0f) + (obj->getSize() * 0.5f));

			// do we really ant to create her body here? humm
			if (_emotion == 99) { // glasses
				SpriteNode* tor_glasses = new SpriteNode();
				tor_glasses->setUndertaleSprite(1989); // glasses
				tor_glasses->setTag(1989); // body
				tor_glasses->setDepth(100); // over
				addChild(tor_glasses);

				_emotion = 0;
			//	RoomObject* obj = instance_create(getPosition().x, 0.0f, 785, 1989); // put in her glasses
			//	Vector2f center(Vector2f(getPosition().x, 0.0f) + (obj->getSize() * 0.5f));
			//	obj->setDepth(101);
			}
		}
		break;

		case 2: // obj_face_floweytalk
			pos = Vector2f(-36.0f, 25.0f);
			break;
		case 3: // obj_face_sans
			pos = Vector2f(-35.0f, 25.0f);
			break;
		case 4: //obj_face_papyrus
			pos = Vector2f(-35.0f, 25.0f);
			break;
		case 5: // obj_face_undyne
			pos = Vector2f(-36.0f, 25.0f);
			break;
		case 6: // obj_face_alphys
			pos = Vector2f(-39.0f, 25.0f);
			break;
		case 7: // obj_face_asgore
			pos = Vector2f(-40.0f, 20.0f);
			break;
		case 8: // obj_face_mettaton
			pos = Vector2f(-18.0f, 25.0f);
			break;
		case 9: // obj_face_asriel
			pos = Vector2f(-30.0f, 30.0f);
			break;
		}
		// not sure if we should set the position or the origin here humm
		// really need some kind of  parent system at some point
	//	_face.setPosition(pos);
		int e = _emotion;
		_emotion = -1; // force update of emotion
		setEmotion(e);
		setImageSpeed(0.25* (1.0f/30.0f));
	}
}

