#pragma once
#include "Global.h"



class BSprite : public sf::Transformable {
	sf::Vertex* _vertices;
	size_t _index;
public:
	BSprite(std::vector<sf::Vertex>& vertices, size_t index) : _vertices(vertices.data() + index), _index(index) {}
	void update() {
		const auto& t = getTransform();
		for (size_t i = 0; i < 6; i++) _vertices[i].position = t.transformPoint(_vertices[i].position);
	}
	void color(const sf::Color& color) {
		for (size_t i = 0; i < 6; i++) _vertices[i].color = color;
	}
	const sf::Color& color() const { return _vertices[0].color; }
};
// simple action class

class Action {
	int _tag;
public:
	virtual ~Action() {}
	int tag() const { return _tag; }
	void tag(int tag) { _tag = tag; }
	virtual bool step(float dt) = 0; // return true if action happend
};

class MoveAction : public Action {
protected:
	sf::Vector2f _velocity;
	sf::Transformable* _node;
public:
	MoveAction(sf::Transformable& node, sf::Vector2f& velocity=sf::Vector2f(0.0f,0.0f)) :  Action(), _node(&node), _velocity(velocity) {}
	const sf::Vector2f& velocity() const { return _velocity; }
	sf::Vector2f& velocity()  { return _velocity; }
	void velocity(const sf::Vector2f& v) { _velocity=v; }
	virtual bool step(float dt) override  { _node->setPosition(_velocity*dt); return true; }
};

class GravityAction : public MoveAction {
protected:
	sf::Vector2f _gravity;
public:
	GravityAction(sf::Transformable& node, sf::Vector2f& gravity) : MoveAction(node), _gravity(gravity) {}
	GravityAction(sf::Transformable& node, float strength) : MoveAction(node), _gravity(CreateMovementVector(90, strength)) {}
	GravityAction(sf::Transformable& node, float direction, float strength) : MoveAction(node), _gravity(CreateMovementVector(direction, strength)) {}
	const sf::Vector2f& gravity() const { return _gravity; }
	sf::Vector2f& gravity() { return _gravity; }
	void gravity(const sf::Vector2f& v) { _gravity = v; }
	void gravity(float direction, float strength) { _gravity = CreateMovementVector(direction, strength); }
	virtual bool step(float dt) override { 
		_velocity += dt * _gravity;
		MoveAction::step(dt);
		return true;
	}
};
class CombinedActions : public Action {
public:
	// we use list here to remove actions if need be
	typedef std::list<std::unique_ptr<Action>>::iterator iterator;
	typedef std::list<std::unique_ptr<Action>>::const_iterator const_iterator;
	typedef std::list<std::unique_ptr<Action>> list_type;
protected:
	std::list<std::unique_ptr<Action>> _actions;
public:
	template<typename R, typename...> struct fst { typedef R type; };

	CombinedActions() : Action() {}
	template<typename C, typename = typename enable_if<is_convertible<Action, C>::value>>
	CombinedActions(std::initializer_list<C> l) : Action() { for (auto action : l) _actions.emplace_back(std::make_unique<Action*>(action)); }


	template<class... Args>
	std::enable_if_t<!std::is_convertible<Action, Args&&...>::value> //Using helper type
	push_back(Args&&... args) {
		std::initializer_list<Action *> list = { &args... };
		for (auto i : list) push_back(i);
	}
	template<typename C, typename = typename enable_if<is_convertible<Action, C>::value>>
	void push_back(C* action) { _actions.push_back(std::move(action)); }

	//template<typename... Args>  void push_back(Action*...rest) { push_back(a); }
//	void push_back(Action* action){ _actions.emplace_back(std::make_unique<Action*>(action)); }
	list_type& actions() { return _actions; }
	const list_type& actions() const { return _actions; }
	
	iterator begin() { return _actions.begin(); }
	const_iterator begin() const { return _actions.begin(); }
	iterator end() { return _actions.end(); }
	const_iterator end() const { return _actions.end(); }
	virtual bool step(float dt) override {
		bool ret = false;
		for (auto& a : _actions) ret |= a->step(dt);
		return ret;
	}
};
class RemoveIfFalseAction : public CombinedActions {
public:
	RemoveIfFalseAction() : CombinedActions() {}
	template<typename C, typename = typename enable_if<is_convertible<Action, C>::value>>
	RemoveIfFalseAction(std::initializer_list<C> l) : CombinedActions(l) {}
	virtual bool step(float dt) override {
		for (auto& it = begin(); it != end(); it++) {
			if (!(*it)->step(dt)) _actions.erase(it);
		}
		return _actions.empty();
	}
};

template<class T> class FunctionAction : public Action {
protected:
	std::function<bool(T& node)> _func;
	T* _tnode;
public:
	FunctionAction(sf::Drawable& node, std::function<bool(T& node)> func) : Action(node), _func(func), _tnode(dynamic_cast<T*>(&_node)) { assert(_tnode != nullptr); }
	void function(std::function<bool(T& node)> func) { _func = func; }
	virtual bool step(float dt) override { return _func(*_tnode); }
};

// simple class that converts a frame into
// vertexes
// The catch is that the offset position moves the center of the frame
// so we have to be sure to put that within the vertexes.  There is no way to do that
// within sf::Sprite as those functions are private 
class GSpriteFrame : public sf::Drawable {
	sf::Vertex  _vertices[6]; ///< Vertices defining the sprite's geometry, should we use quads?
	struct SpriteFrameCache {
		sf::IntRect rect;
		sf::Texture* texture;
		sf::Vertex  vertices[6];
		~SpriteFrameCache();
	};
	std::shared_ptr<SpriteFrameCache> _frame;
	friend class SpriteFrameCacheHelper;
public:
	GSpriteFrame(const Undertale::SpriteFrame* frame, sf::Color color = sf::Color::White);
	GSpriteFrame()  {}
	void setColor(const sf::Color& color);
	void setFrame(const Undertale::SpriteFrame* frame);
	void insertIntoVertexList(sf::VertexArray& list) const;
	inline void insertIntoVertexList(sf::Vertex* dist) const { std::memcpy(dist, _vertices, sizeof(sf::Vertex) * 6); }

	const sf::IntRect& getTextureRect() const;
	const sf::Texture* getTexture() const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
struct Contact {
	sf::Vector2f normal;
	float distance;
};
// Each frame we update the position of movmenet
// if you try to set 
class RigidBody{// : public sf::Transformable {
	sf::Transformable* _sprite;
	float _invMass;
	CC_SYNTHESIZE_REF(sf::Vector2f, _velocity, VelocityVector);
	CC_SYNTHESIZE_REF(sf::Vector2f, _gravityVector, GravityVector);
	CC_SYNTHESIZE_READONLY(float, _mass, Mass);
public: 
	sf::Transformable* getTransform() const {return _sprite; };
	RigidBody(sf::Transformable* sprite) : _mass(0), _sprite(sprite) {}
	void setMass(float mass) { _mass = mass; _invMass = 1.0f / mass; }
	virtual void step(float dt) {
		sf::Vector2f pos = getNextPosition(dt);
		if (!fast::is_zero(_gravityVector)) {	//update
			_velocity += dt * _gravityVector; // velocity += timestep * acceleration;	
		}
		_sprite->setPosition(pos);
	}
	const sf::Vector2f getNextPosition(sf::Vector2f pos, float dt) const {
		pos += _velocity; // add the movment vector first
		pos += _gravityVector; // then add the gravity
		return pos;
	}
	const sf::Vector2f getNextPosition(float dt) const {
		return getNextPosition(_sprite->getPosition(), dt);
	}
	void setVelocity(float direction, float speed) { _velocity = CreateMovementVector(direction, speed); }
	void setGravity(float dir,float strength) { _gravityVector = CreateMovementVector(dir,strength); }
	float getSpeed() const { return MagnatudeOfVector(_velocity); }
	float getDirection() const { return AngleOfVector(_velocity); }
};

class RigidBodySprite : public sf::Sprite {
	RigidBody _body;
public:
	RigidBody& getBody()  { return _body; }
	const RigidBody& getBody() const { return _body; }
	RigidBodySprite() : _body(this), sf::Sprite() {}
	RigidBodySprite(const sf::Texture& texture) : _body(this), sf::Sprite(texture) {}
	RigidBodySprite(const sf::Sprite& sprite) : _body(this), sf::Sprite(sprite) {}
	virtual void step(float dt) { _body.step(dt); }
};

class GSprite : public sf::Drawable, public sf::Transformable {
	RigidBody _body;
	const Undertale::Sprite* _sprite;
	int _image_index;
	sf::Color _color;
	GSpriteFrame _frame;///< Texture of the sprite
public:

	GSprite() : _body(this), _sprite(nullptr), _image_index(0), _color(sf::Color::White) {}
	GSprite(int sprite_index, int image_index = 0):_body(this), _color(sf::Color::White) { setUndertaleSprite(sprite_index); setImageIndex(0); }
	GSprite(const std::string& name, int image_index = 0) : _body(this), _color(sf::Color::White) { setUndertaleSprite(name); setImageIndex(0); }
	GSprite(const Undertale::Sprite* sprite, int image_index = 0) :_body(this), _color(sf::Color::White) { setUndertaleSprite(sprite); setImageIndex(0); }

	void setUndertaleSprite(int index);
	void setUndertaleSprite(const std::string& name);
	void setUndertaleSprite(const Undertale::Sprite* sprite);
	virtual void step(float dt) {
		_body.step(dt);
	}
	void setColor(const sf::Color& color) { _frame.setColor(_color = color); }
	const sf::Color& getColor() const { return _color; }
	sf::Vector2f getSize() const { return sf::Vector2f(_sprite->width()*getScale().x, _sprite->height()*getScale().y); }
	sf::FloatRect getBounds() const { return sf::FloatRect(getPosition(), getSize()); }
	const sf::IntRect& getTextureRect() const { return _frame.getTextureRect(); }
	const sf::Texture* getTexture() const { return _frame.getTexture(); }

	void setImageIndex(int index) { _frame.setFrame(_sprite->frames()->at(_image_index = _sprite ? index % _sprite->frames()->size() : 0)); }
	const char* getName() const { return _sprite->name().c_str(); }
	uint32_t getIndex() const { return _sprite->index(); }
	
	int getImageIndex() const { return _image_index; }
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (_sprite) {
			states.transform *= getTransform();
			target.draw(_frame, states);
		}
	}
};