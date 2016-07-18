#include "Ref.h"
#include "Global.h"
#include <type_traits>
#include <cassert>

void Ref::retain() { 
	assert(_refCount > 0); ++_refCount; }
void Ref::release() { 
	assert(_refCount > 0);
	--_refCount; 
	auto poolManager = PoolManager::getInstance();
	if (poolManager->isObjectInPools(this))
	{
		assert(false, "The reference shouldn't be 0 because it is still in autorelease pool.");
	}

	if (_refCount == 0) delete this; 
}

Ref* Ref::autorelease() {
	PoolManager::getInstance()->getCurrentPool()->addObject(this);
	return this;
}


AutoreleasePool::AutoreleasePool()
	: _name("")
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
	, _isClearing(false)
#endif
{
	_managedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoreleasePool::AutoreleasePool(const std::string &name)
	: _name(name)
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
	, _isClearing(false)
#endif
{
	_managedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoreleasePool::~AutoreleasePool()
{
//	CCLOGINFO("deallocing AutoreleasePool: %p", this);
	clear();

	PoolManager::getInstance()->pop();
}

void AutoreleasePool::addObject(Ref* object)
{
	_managedObjectArray.push_back(object);
}

void AutoreleasePool::clear()
{

	std::vector<Ref*> releasings;
	releasings.swap(_managedObjectArray);
	for (const auto &obj : releasings)obj->release();
}

bool AutoreleasePool::contains(Ref* object) const
{
	for (const auto& obj : _managedObjectArray)
	{
		if (obj == object) return true;
	}
	return false;
}

void AutoreleasePool::dump()
{
	console::info("autorelease pool: %s, number of managed object %d\n", _name.c_str(), static_cast<int>(_managedObjectArray.size()));
	console::info("%20s%20s%20s", "Object pointer", "Object id", "reference count");
	for (const auto &obj : _managedObjectArray)
	{
		//CC_UNUSED_PARAM(obj);
		console::info("%20p%20u\n", obj, obj->getReferenceCount());
	}
}


//--------------------------------------------------------------------
//
// PoolManager
//
//--------------------------------------------------------------------

PoolManager* PoolManager::s_singleInstance = nullptr;

PoolManager* PoolManager::getInstance()
{
	if (s_singleInstance == nullptr)
	{
		s_singleInstance = new (std::nothrow) PoolManager();
		// Add the first auto release pool
		new AutoreleasePool("cocos2d autorelease pool");
	}
	return s_singleInstance;
}

void PoolManager::destroyInstance()
{
	delete s_singleInstance;
	s_singleInstance = nullptr;
}

PoolManager::PoolManager()
{
	_releasePoolStack.reserve(10);
}

PoolManager::~PoolManager()
{
	//CCLOGINFO("deallocing PoolManager: %p", this);

	while (!_releasePoolStack.empty())
	{
		AutoreleasePool* pool = _releasePoolStack.back();

		delete pool;
	}
}


AutoreleasePool* PoolManager::getCurrentPool() const
{
	return _releasePoolStack.back();
}

bool PoolManager::isObjectInPools(Ref* obj) const
{
	for (const auto& pool : _releasePoolStack)
	{
		if (pool->contains(obj))
			return true;
	}
	return false;
}

void PoolManager::push(AutoreleasePool *pool)
{
	_releasePoolStack.push_back(pool);
}

void PoolManager::pop()
{
	assert(!_releasePoolStack.empty());
	_releasePoolStack.pop_back();
}
