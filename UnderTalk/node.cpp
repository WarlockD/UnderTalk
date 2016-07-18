#include "node.h"
#include "room.h"


Room* Node::getRoom() const {
	if (!getParent()) return nullptr;
	Node* node = dynamic_cast<Node*>(getParent());
	if (node != nullptr) return node->getRoom();
	else return dynamic_cast<Room*>(getParent());
	// is this slow?
}

void Node::resortChildren() const {
	if (_zOrderDirty) {
		std::sort(_children.begin(), _children.end(), [](const Node*  l, const Node*  r) { return r->_depth < r->_depth; });
		_zOrderDirty = false;
	}
}


void Node::setParent(Node* node) {
	if (getParent() == node) return;
	if (_parent != nullptr) _parent->removeChild(this);
	if (node != nullptr) node->addChild(this);
}
void Node::addChild(Node* child) {
	assert(child != nullptr);
	assert(child->getParent() == nullptr);
	_children.push_back(child);
	child->retain();
	child->_parent = this;
}
void Node::removeChildHelper(Node* child){
	assert(child->_parent == this);
	child->_parent = nullptr;
	child->release();
}
void Node::addChildHelper(Node* child) {
	assert(child->_parent == nullptr);
	child->_parent = this;
	child->retain();
	_zOrderDirty = true;
}

template<class C> C* Node::findChild(std::function<bool(const Ref*)> pred) const {
	for (auto& it = _children.begin(); it != _children.end(); it++) {
		C* child = dynamic_cast<C*>((*it));
		if (child != nullptr && pred(child)) return child;
	}
	return nullptr;
}
template<class C> 
bool Node::findChildren(std::vector<C*>& search, std::function<bool(const C*)> pred) const {
	bool found = false;
	for (auto& it = _children.begin(); it != _children.end(); it++) {
		C* child = dynamic_cast<C*>(*it);
		if (child != nullptr && pred(child)) { 
			search.push_back(child); 
			found = true; 
		}
	}
	return found;
}


template<class C> bool Node::findChildrenByTag(std::vector<C*>& search, int tag) const {
	return findChildren(search, [tag](const C* c) { return c->getTag() == tag; });
}


template<class C> void Node::with(int tag, std::function<void(C*)> func) {
	for (auto& it = _children.begin(); it != _children.end(); it++) {
		C* child = dynamic_cast<C*>((*it));
		if (child != nullptr && child->getTag() == tag) func(child); 
	}
}

void Node::removeChild(int tag) {
	removeChild<Node>([tag](const Node* ref) { return ref->getTag() == tag; });
}

void Node::removeChild(const std::type_info& type) {
	auto it = _children.begin();
	while (it != _children.end()) {
		Node* child = (*it);
		if (typeid((*it)) == type) {
			removeChildHelper(child);
			it = _children.erase(it);
		}
		else it++;
	}
}
void Node::removeChild(Node* child) {
	removeChild<Node>([child](const Node* ref) { return ref == child; });
}

template<> void Node::removeChild(std::function<bool(const Node*)> pred) {
	auto it = _children.begin();
	while (it != _children.end()) {
		Node* child = (*it);
		if (pred(child)) {
			removeChildHelper(child);
			it = _children.erase(it);
		}
		else it++;
	}
}

void Node::step(float dt) {
	if (_children.empty()) return;
	bodyStep(dt);
	for (auto child : _children) child->step(dt);
	if (_zOrderDirty) resortChildren();
}
void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (_children.empty()) return;
	for (auto child : _children) target.draw(*child, states);
}