#include "obj_face.h"

using namespace sf;


void obj_face::setEmotion(size_t e) { 
	if (e != _emotion) {
		_emotion = e;
		switch (_choice) {
		case 1: // obj_face_torieltalk
			switch (_emotion) {
			case 0: _face.setUndertaleSprite(1986); break;
			case 1: _face.setUndertaleSprite(2004); break;
			case 2: _face.setUndertaleSprite(1990); break;
			case 3: _face.setUndertaleSprite(1999); break;
			case 4: _face.setUndertaleSprite(2000); break;
				// case 5: missing? humm
			case 6: _face.setUndertaleSprite(1991); break;
			case 7: _face.setUndertaleSprite(1993); break;
			case 8: _face.setUndertaleSprite(1996); break;
			case 9: _face.setUndertaleSprite(1987); break;
				break;
			}
			break;
		case 2: // obj_face_floweytalk
			switch (_emotion) {
			case 0: _face.setUndertaleSprite(166); break;
			case 1: _face.setUndertaleSprite(170); break;
			case 2: _face.setUndertaleSprite(171); break;
			case 3: _face.setUndertaleSprite(172); break;
			case 4: _face.setUndertaleSprite(179); break;
			case 5: _face.setUndertaleSprite(175); break;
			case 6: _face.setUndertaleSprite(176); break;
			case 7: _face.setUndertaleSprite(177); break;
			case 8: _face.setUndertaleSprite(167); break;
		//	case 9:  break;
			}
			break;
		case 3: // obj_face_sans
			switch (_emotion) {
			case 0: _face.setUndertaleSprite(2018); break;
			case 1: _face.setUndertaleSprite(2022); break;
			case 2: _face.setUndertaleSprite(2019); break;
			case 3: _face.setUndertaleSprite(2020); break;
			case 4: _face.setUndertaleSprite(2021); break;
			}
			break;
		case 4: //obj_face_papyrus
			switch (_emotion) {
			case 0: _face.setUndertaleSprite(2008); break;
			case 1: _face.setUndertaleSprite(2010); break;
			case 2: _face.setUndertaleSprite(2012); break;
			case 3: _face.setUndertaleSprite(2013); break;
			case 4: _face.setUndertaleSprite(2011); break;
			case 5: _face.setUndertaleSprite(2009); break;
			case 6: _face.setUndertaleSprite(2014); break;
			case 7: _face.setUndertaleSprite(2015); break;
			case 8: _face.setUndertaleSprite(2016); break;
			case 9: _face.setUndertaleSprite(2017); break;
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
void obj_face::setFaceChoice(size_t c) { 
	if (c != _choice) {
		if (_choice == 1) {
			removeChild(1985); // body
			removeChild(1989); // glasses
		}
		_choice = c;
		sf::Vector2f pos;
		switch (_choice) {
		case 1: // obj_face_torieltalk
		{
			pos = Vector2f(-33.0f, 25.0f);
			RoomObject* obj = instance_create(getPosition().x, 0.0f, 785, 1985);  // body
			Vector2f center(Vector2f(getPosition().x, 0.0f) + (obj->getSize() * 0.5f));
			obj->setDepth(100);

			// do we really ant to create her body here? humm
			if (_emotion == 99) {
				_emotion = 0;
				RoomObject* obj = instance_create(getPosition().x, 0.0f, 785, 1989); // put in her glasses
				Vector2f center(Vector2f(getPosition().x, 0.0f) + (obj->getSize() * 0.5f));
				obj->setDepth(101);
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
		_face.setPosition(pos);
		int e = _emotion;
		_emotion = -1; // force update of emotion
		setEmotion(e);
		setImageSpeed(0.25* (1.0f/30.0f));
	}
}

