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
inline float FixAngleRange(float angle) {
	return angle >= 180.0f ? (angle - 360.0f) : angle < -180.f ? (angle + 360.0f) : angle;
}
inline float getAngleDifference(float angle1, float angle2)
{
	float diffAngle = (angle1 - angle2);

	return FixAngleRange(diffAngle); // how much angle the node needs to rotate
}
inline float getCurrentAngle(sf::Transformable* node)
{
	float rotAng = node->getRotation();
	// negative angle means node is facing to its left
	// positive angle means node is facing to its right
	return FixAngleRange(rotAng); ;
}
template<typename T> inline T AngleOfVector(const sf::Vector2<T>&  v) {
	// simple angles and save devide by zero issues, bad return if the vector is 0
	if (v.x == (T)0) return v.y > (T)0 ? (T)270 : (T)90;
	else if (v.y == (T)0) return v.x > (T)0 ? (T)0 : (T)180;
	else {
		float rot = std::tanhf((float)v.y / (float)v.x);
		return rot >= 180.0f ? (T)(rot - 360.0f) : (T)(rot + 360.0f);
	}
}

template<typename T> inline T MagnatudeOfVector(const sf::Vector2<T>& v) {
	return (T)std::sqrtf((float)v.x * (float)v.x + (float)v.y * (float)v.y);
}



void MakeSpriteTriangleStrip(sf::Vertex* vertices, const sf::IntRect& textRect, const sf::Color& color, const sf::Vector2f& offset, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));
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


typedef std::shared_ptr<sf::Texture> SharedTexture;
namespace Undertale {
	//SharedTexture::TextureInfo GetTexture(int index,const sf::IntRect& rect);
	const std::map<int, sf::Glyph>& GetFontGlyphs(int font_index);
	const sf::Texture* GetFontTexture(int font_index);
	int GetFontSize(int font_index);
	const sf::Image& GetTextureImage(int index);
	SharedTexture GetTexture(uint32_t index);
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

// http://stackoverflow.com/questions/427477/fastest-way-to-clamp-a-real-fixed-floating-point-value
// such a cool method, not sure its faster than normal but thought I mess with it 
#include <smmintrin.h>
namespace fast {
	inline float min(float a, float b)// Branchless SSE min.
	{
		_mm_store_ss(&a, _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}
#if 0
	// not sure how masks work yet
	inline float epsilonequal(float a, float b)// Branchless SSE min.
	{
		__declspec(align(16)) static const float epsilon = std::numeric_limits<float>::epsilon();
		return _mm_cmplt_ss(_mm_sub_ss(_mm_set_ss(a), _mm_set_ss(b)), _mm_set_ss(epsilon)) != 0;
	}
	inline float epsilonequal(float a, float b, float epsilon)// Branchless SSE min.
	{
		return _mm_cmplt_ss(_mm_sub_ss(_mm_set_ss(a), _mm_set_ss(b)), _mm_set_ss(epsilon)) != 0;
	}
#endif
	inline float max(float a, float b) // Branchless SSE max.
	{
		_mm_store_ss(&a, _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}
	inline float clamp(float val, float minval, float maxval)
	{
		// Branchless SSE clamp.
		// return minss( maxss(val,minval), maxval );
		_mm_store_ss(&val, _mm_min_ss(_mm_max_ss(_mm_set_ss(val), _mm_set_ss(minval)), _mm_set_ss(maxval)));
		return val;
	}

	inline bool equal(float a, float b, float epsilon) { return std::fabsf(a - b) < epsilon; }
	inline bool equal(float a, float b) { return equal(a, b, std::numeric_limits<float>::epsilon()); }
	inline bool is_zero(float f, float epsilon) { return std::fabsf(f) < epsilon; }
	inline bool is_zero(float f) { return is_zero(f, std::numeric_limits<float>::epsilon()); }
	inline bool is_zero(const sf::Vector2<float>& v, float epsilon) { return is_zero(v.x, epsilon) && is_zero(v.y, epsilon); }
	inline bool is_zero(const sf::Vector2<float>& v) { return is_zero(v.x) && is_zero(v.y); }
	inline bool equal(const sf::Vector2<float>&  a, const sf::Vector2<float>&  b, float epsilon) { return equal(a.x, b.x, epsilon) && equal(a.y, b.y, epsilon); }
	inline bool equal(const sf::Vector2<float>&  a, const sf::Vector2<float>&  b) { return equal(a.x, b.x) && equal(a.y, b.y); }
	inline sf::Vector2<float> round(const sf::Vector2<float>& v) { return sf::Vector2<float>(std::roundf(v.x), std::roundf(v.y)); }
	inline sf::Vector2<float> floor(const sf::Vector2<float>& v) { return sf::Vector2<float>(std::floorf(v.x), std::floorf(v.y)); }
	template<typename T> inline T random(T min, T max) { return  min + static_cast <T> (std::rand()) / (static_cast <T> (RAND_MAX / (max - min))); }
	template<typename T> inline T random(T max) { return random((T)0, max); }
	uint32_t random();
	void set_random_seed(uint32_t seed);
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



namespace console {
	/// The max length of CCLog message.
	void info(const char* format, ...);
	void error(const char * format, ...);
}

