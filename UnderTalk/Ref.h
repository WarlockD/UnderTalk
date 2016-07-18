#pragma once
#include <vector>
#include <string>

class Ref {
	unsigned int  _refCount;
public:
	unsigned int Ref::getReferenceCount() const { return _refCount; }
	Ref() :_refCount(1) {}
	virtual ~Ref() {  }
	void retain();
	void release();
	Ref* autorelease();
};

class  AutoreleasePool
{
public:
	/// warning Don't create an autorelease pool in heap, create it in stack.
	AutoreleasePool();
	AutoreleasePool(const std::string &name);
	~AutoreleasePool();
	void addObject(Ref *object);
	void clear();
	bool contains(Ref* object) const;
	void dump();
private:
	std::vector<Ref*> _managedObjectArray;
	std::string _name;
};

class  PoolManager
{
public:
	AutoreleasePool *getCurrentPool() const;
	bool isObjectInPools(Ref* obj) const;
	static PoolManager* getInstance();
	static void destroyInstance();
	friend class AutoreleasePool;
private:
	PoolManager();
	~PoolManager();
	void push(AutoreleasePool *pool);
	void pop();
	static PoolManager* s_singleInstance;
	std::vector<AutoreleasePool*> _releasePoolStack;
};
