#pragma once
#include "Global.h"
#include <typeinfo>
#include "Ref.h"
// a hacked implmentation of the node in cocos2d-x
class Room;


// All nodes draw, otherwise what would be the point?
class Node : public Ref , public sf::Drawable , public sf::Transformable {
	int _tag;
	int _depth;
	sf::Vector2f _movmentVector;
	sf::Vector2f _gravityVector;
	sf::Vector2f _velocityVector;
	float _gravity;
	float _gravityDirection;
	float _direction;
	float _speed;
	sf::Vector2f _size;
	// mabye use above if we want to control the managment of pointers
//	typedef set_unique_ptr<Ref> t_child;
	//typedef std::unique_ptr<Ref> t_child;
	typedef Node* t_child;
	mutable std::vector<t_child> _children;
	mutable bool _zOrderDirty;
	// finds the parent in the chain, if we cannot find it than we are screwed
	void removeChildHelper(Node* child);
	void addChildHelper(Node* child);
	Node* _parent;
public:
	int getTag() const { return _tag; }
	void setTag(int tag) { _tag = tag; }
	int getDepth() const { return _depth; }
	void setDepth(int depth) { _depth = depth; }
	void setDirection(float d) { _movmentVector = CreateMovementVector(_direction = d, _speed); }
	void setSpeed(float s) { _movmentVector = CreateMovementVector(_direction, _speed = s); }
	void setGravityDirection(float d) { _gravityVector = CreateMovementVector(_direction = d, _gravity);  _velocityVector = sf::Vector2f(); }
	void setGravity(float s) { _gravityVector = CreateMovementVector(_direction, _gravity = s); _velocityVector = sf::Vector2f(); }
	float getGravity() const { return _gravity; }
	float getSpeed() const { return _speed; }

	const sf::Vector2f& getMovement() const { return _movmentVector; }
	sf::Vector2f& getMovement() { return _movmentVector; }
	const sf::Vector2f& getVelocity() const { return _velocityVector; }
	sf::Vector2f& getVelocity() { return _velocityVector; }
	const sf::Vector2f& getSize() const { return _size; }
	void setSize(const sf::Vector2f& size) { _size = size; }

	const sf::Vector2f getNextPosition(float dt) const {
		sf::Vector2f pos = getPosition();
		pos += _movmentVector  * dt; // add the movment vector first
		pos += _velocityVector  * dt; // add the gravity acceration
		return pos;
	}
	void bodyStep(float dt);
public:

	Node() : Ref(), _parent(nullptr), _zOrderDirty(false) {}
	Node* getParent() const { return _parent; }
	void setParent(Node* node);

	const std::vector<t_child>& getChildren() const { resortChildren(); return _children; }
	void removeAllChildren() { _children.clear(); _zOrderDirty = false; }
	std::vector<t_child>::iterator begin() { return _children.begin(); }
	std::vector<t_child>::iterator end() {  return _children.end(); }
	const std::vector<t_child>::iterator begin() const {  return _children.begin(); }
	const std::vector<t_child>::iterator end() const {  return _children.end(); }

	void resortChildren() const;
	

	Room* getRoom() const;
	

	template<class C> C* findChild(std::function<bool(const Ref*)> pred) const;
	template<class C> bool findChildren(std::vector<C*>& search, std::function<bool(const C*)> pred) const;

	template<class C> C* findByTag(int tag) const;
	template<class C> bool findChildrenByTag(std::vector<C*>& search, int tag) const;
	template<class C> void with(int tag, std::function<void(C*)> func);

	template<class C> void removeChild(std::function<bool(const C*)> pred);
	template<> void Node::removeChild(std::function<bool(const Node*)> pred);


	void addChild(Node* child);
	void removeChild(int tag);
	void removeChild(Node* child);
	void removeChild(const std::type_info& type);


	virtual void step(float dt) ;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};