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
#include <random>
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

void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));
inline void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	MakeSpriteTriangleStrip(vertices, textRect, sf::Color::White, offset, scale);
}
void AppendSpriteTriangleStrip(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));
void AppendSpriteTriangleStrip(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));
inline void AppendSpriteTriangleStrip(sf::VertexArray& vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	AppendSpriteTriangleStrip(vertices, textRect, sf::Color::White, offset, scale);
}
inline void AppendSpriteTriangleStrip(std::vector<sf::Vertex>& vertices, const sf::IntRect& textRect, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f)) {
	AppendSpriteTriangleStrip(vertices, textRect, sf::Color::White, offset, scale);
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

namespace std {
	template<> struct hash<sf::IntRect> {
		std::size_t operator()(const sf::IntRect& k) const
		{  // this hash should cover most of it, if we have an x, y it covers it, and if we don't width and height pick up the slack
			// I am a hash god!
			return ((k.width & 0xFFFF) << 16 | (k.height & 0xFFFF)) ^ ((k.left & 0xFFFF) << 16 | k.top&0xFFFF);
		}
	};
}


class SharedTexture {
	struct TextureRef {
		sf::Texture texture;
		sf::IntRect frame;
	};
	sf::Image _image;
	std::shared_ptr<TextureRef> _fullTexture; // if we can load the entire image, than save it here, otherwise we have to count it
	std::unordered_map<sf::IntRect, std::weak_ptr<TextureRef>> _textures;
	void checkOpenGL();
public:
	class TextureInfo {
		const SharedTexture* _owner;
		std::shared_ptr<TextureRef> _texture;
		sf::IntRect _rect;
		friend class SharedTexture;
		TextureInfo(const SharedTexture* owner, std::shared_ptr<TextureRef>  texture, const sf::IntRect& rect) : _owner(owner), _texture(texture), _rect(rect) {}
	public:
		TextureInfo() : _owner(nullptr) {}
		const sf::Texture* getTexture() const { return &_texture->texture; }
		const sf::IntRect& getFrame() const { return _rect; }
		const sf::Image& getOriginalImage() const { return _owner->getImage(); }
		sf::Image copyToImage() const {
			const sf::IntRect& rect = _texture->frame;
			sf::Image image;
			image.create(rect.width, rect.height);
			image.copy(getOriginalImage(), rect.top, rect.left, rect, true);
			return std::move(image);
		}
	};
	SharedTexture() :_fullTexture(nullptr) {}
	SharedTexture(const sf::Image& image) : _image(image) , _fullTexture(nullptr) { checkOpenGL(); }
	SharedTexture(sf::Image&& image) : _image(image) , _fullTexture(nullptr) { checkOpenGL(); }

	void setImage(sf::Image&& image) { assert(_textures.empty());  _image = image; checkOpenGL();}
	void setImage(const sf::Image& image) { assert(_textures.empty()); _image = image; checkOpenGL();}

	const sf::Image& getImage() const { return _image; }
	// attempts to request a texture.  If we cannot load the whole thing, then we cut the texture
	// int to a partial texture and give you that
	TextureInfo requestTexture(const sf::IntRect& frame);
	// it will try to create one texture that holds multipul frames. throws if fucked
	std::shared_ptr<sf::Texture> requestTexture(const std::initializer_list<sf::IntRect>& frames);
};

namespace Undertale {
	SharedTexture::TextureInfo GetTexture(int index,const sf::IntRect& rect);
	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index);
	const sf::Texture* GetFontTexture(int font_index);
	int GetFontSize(int font_index);
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

template<typename Tval>
struct BetterPointerHasher {
	size_t operator()(const Tval* val) const {
		static const size_t shift = (size_t)log2(1 + sizeof(Tval));
		return (size_t)(val) >> shift;
	}
};

class Randomizer {
private:
	std::random_device device_;
	std::default_random_engine engine_;
public:
	Randomizer() : device_(), engine_(device_()) {};
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type rnd(T a, T b) {
		std::uniform_int_distribution<T> uni_dist(a, b);
		return uni_dist(engine_);
	}
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type rnd(T a) {
		std::uniform_int_distribution<T> uni_dist(((T)0), a);
		return uni_dist(engine_);
	}
	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type rnd(T a, T b) {
		std::uniform_real<T> uni_dist(a, b);
		return uni_dist(engine_);
	}
	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type rnd(T a) {
		std::uniform_real<T> uni_dist(((T)0), a);
		return uni_dist(engine_);
	}
};
/*
// loveing trates
template<class T>
bool isEqual(T a, T b, typename std::enable_if<std::is_integral<T>::value >::type* = 0)
{
	return a == b;
}
template<class T>
typename std::enable_if<std::is_integral<T>::value, bool >::type isEqual(T a, T b)

{
return a == b;
}
*/

namespace util {
	template<typename T> T random(T a, T b);
	
	template<typename T> T random(T a);
	template<> float random(float a);
	template<> float random(float a, float b);
};

struct ITimeStep {
	virtual int step(float dt) = 0;
	virtual ~ITimeStep() {}
};

class Rigidbody {
	sf::Vector2f _velocity;
	sf::Vector2f _gravity;
	float _speed;
	float _direction;
public:
	Rigidbody() : _velocity(0.0f, 0.0f), _gravity(0.0f, 0.0f) {}
	Rigidbody(const sf::Vector2f& velocity) : _velocity(velocity), _gravity(0.0f, 0.0f) {}
	Rigidbody(const sf::Vector2f& velocity, const sf::Vector2f& gravity) : _velocity(velocity), _gravity(gravity) {}
	virtual ~Rigidbody() {}
	const sf::Vector2f& getVelocity() const { return _velocity; }
	sf::Vector2f& getVelocity() { return _velocity; }
	const sf::Vector2f& getGravity() const { return _gravity; }
	sf::Vector2f& getGravity() { return _gravity; }
	void setVelocity(const sf::Vector2f& v) { _velocity = v; }
	void setGravity(const sf::Vector2f& v) { _gravity = v; }
	void setVelocity(float x, float y) { setVelocity(sf::Vector2f(x, y)); }
	void setGravity(float x, float y) { setGravity(sf::Vector2f(x, y)); }
	void setSpeed(float speed) { _velocity = CreateMovementVector(_direction, (_speed = speed)); }
	void setDirection(float dir) { _velocity = CreateMovementVector((_direction = dir), _speed); }
	float getSpeed() const { return _speed; }
	float getDirection() const { return _direction; }
	template<class C> void step(float dt, C& node) {
		sf::Vector2f pos = node.getPosition();
		pos += _velocity;
		_velocity += _gravity;
		node.setPosition(pos);
	}
	void step(float dt, sf::Vertex* vertices, size_t count) {
		for (size_t i = 0; i < count; i++) vertices[i].position += _velocity;
		_velocity += _gravity;
	}
};
