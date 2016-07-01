#pragma once
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "json.hpp"
#include <fstream>
#include <map>
#include <UndertaleLib.h>

// interfaces used when you need to run before or after the main step
inline float lengthdir_x(float len, float dir) {
	const double pi = 3.1415926535897;
	return std::cosf(dir* 0.01745329252f) * len;
}
inline float lengthdir_y(float len, float dir) {
	const float rad = 0.01745329252f;
	return -std::sinf(dir* rad) * len;
}
inline sf::Vector2f CreateDirectionVector(float dir) {
	const float rad = 0.01745329252f;
	float x = std::cosf(dir* rad);
	float y = std::sinf(dir* rad);
	return sf::Vector2f(x, y);
}
inline sf::Vector2f CreateMovementVector(float dir, float speed) {
	const float rad = 0.01745329252f;
	float x = std::cosf(dir* rad) * speed;
	float y = std::sinf(dir* rad) * speed;
	return sf::Vector2f(x, y);
}
inline sf::Vector2f CreateMovementVector(float dir, sf::Vector2f speed) {
	const float rad = 0.01745329252f;
	float x = std::cosf(dir* rad) * speed.x;
	float y = std::sinf(dir* rad) * speed.y;
	return sf::Vector2f(x, y);
}
// http://discuss.cocos2d-x.org/t/rotate-sprite-towards-a-point/16850/2
// useful functions
inline float getAngleDifference(float angle1, float angle2)
{
	float diffAngle = (angle1 - angle2);

	if (diffAngle >= 180.f)
	{
		diffAngle -= 360.f;
	}
	else if (diffAngle < -180.f)
	{
		diffAngle += 360.f;
	}

	// how much angle the node needs to rotate
	return diffAngle;
}
inline float getCurrentAngle(sf::Transformable* node)
{
	float rotAng = node->getRotation();

	if (rotAng >= 180.f)
	{
		rotAng -= 360.f;
	}
	else if (rotAng < -180.f)
	{
		rotAng += 360.f;
	}

	// negative angle means node is facing to its left
	// positive angle means node is facing to its right
	return rotAng;
}
void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f,1.0f));

void AppendSpriteTriangles(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));
void AppendSpriteTriangles(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));

inline void MakeSpriteTriangles(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	MakeSpriteTriangles(vertices, textRect, sf::Color::White, offset, scale);
}
inline void AppendSpriteTriangles(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	AppendSpriteTriangles(vertices, textRect, sf::Color::White, offset, scale);
}
inline void AppendSpriteTriangles(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	AppendSpriteTriangles(vertices, textRect, sf::Color::White, offset, scale);
}


void LoadUndertaleResources(const std::string& filename);
Undertale::UndertaleFile& GetUndertale();
const sf::Sprite& GetUndertaleSprite(int index,int frame=0);



namespace Undertale {
	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index);
	const sf::Texture& GetFontTexture(int font_index);
	int GetFontSize(int font_index);
	const sf::Texture* GetTexture(int index);
	const sf::Image* GetTextureImage(int index);
	const std::string& LookupSound(int index);
	void LoadAllFonts();
}

// I have always liked these macros from cocos2d

#define CC_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }\
public: inline void set##funName(varType var){ varName = var; }

#define CC_SYNTHESIZE_REF(varType, varName, funName)\
protected: varType varName;\
public: inline const varType& get##funName(void) const { return varName; }\
public: inline varType& get##funName(void) { return varName; }\
public: inline void set##funName(varType var){ varName = var; }

#define CC_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }


#define CC_SYNTHESIZE_REF_READONLY(varType, varName, funName)\
protected: varType varName;\
public: inline const varType& get##funName(void) const { return varName; }
