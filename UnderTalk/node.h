#pragma once
#include "Global.h"
#include <typeinfo>

// a hacked implmentation of the node in cocos2d-x
class Room;
class Node;
// Ref is a light weight ref counted drawable object
class Ref : public sf::Drawable {
	friend class Node;
	int _ref;
	int _tag;
	int _depth;
	Node* _parent; // make above shared ptr and this a weak ptr? humm.  Now it should all be unique ptrs
public:
	Ref() :_ref(1), _tag(0), _depth(0) , _parent(nullptr)  {}
	virtual ~Ref() { if(_parent!= nullptr) setParent(nullptr); }
	int getTag() const { return _tag; }
	void setTag(int tag) { _tag = tag; }
	int getDepth() const { return _depth; }
	void setDepth(int depth) { _depth = depth; }
	void setParent(Node* node);
	void retain() { assert(_ref > 0); ++_ref; }
	void release() { assert(_ref > 0); --_ref; if (_ref == 0) { setParent(nullptr);  delete this; } }
	Node* getParent() const { return _parent; }
};
// All nodes draw, otherwise what would be the point?
class Node : public Ref {
	friend class Ref;

	// mabye use above if we want to control the managment of pointers
//	typedef set_unique_ptr<Ref> t_child;
	//typedef std::unique_ptr<Ref> t_child;
	typedef Ref* t_child;
	mutable std::vector<Ref*> _children;
	mutable bool _zOrderDirty;
	// finds the parent in the chain, if we cannot find it than we are screwed
	void removeChildHelper(Ref* child);
	void addChildHelper(Ref* child);
public:
	Node() : Ref(), _zOrderDirty(false) {}
	const std::vector<t_child>& getChildren() const { resortChildren(); return _children; }
	void removeAllChildren() { _children.clear(); _zOrderDirty = false; }
	std::vector<t_child>::iterator begin() { return _children.begin(); }
	std::vector<t_child>::iterator end() {  return _children.end(); }
	const std::vector<t_child>::iterator begin() const {  return _children.begin(); }
	const std::vector<t_child>::iterator end() const {  return _children.end(); }

	void resortChildren() const;
	

	Room* getRoom() const;
	void addChild(Ref* child);

	template<class C> 
	C* findChild(std::function<bool(const Ref*)> pred) const;

	template<class C> 
	bool findChildren(std::vector<C*>& search, std::function<bool(const C*)> pred) const;

	template<class C> C* findByTag(int tag) const;
	template<class C> bool findChildrenByTag(std::vector<C*>& search, int tag) const;
	template<class C> void with(int tag, std::function<void(C*)> func);

	template<class C> void removeChild(std::function<bool(const C*)> pred);
	template<> void Node::removeChild(std::function<bool(const Ref*)> pred);
	void removeChild(int tag);
	void removeChild(Ref* child);
	void removeChild(const std::type_info& type);
	virtual void step(float dt) { if (_zOrderDirty) resortChildren(); }
	virtual void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const;
};