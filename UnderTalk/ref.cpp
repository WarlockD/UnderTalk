#include "Ref.h"
#include "Global.h"
#include <type_traits>
#include <cassert>
#include <mutex>



void Ref::retain() { 
	++_refCount;
}
void Ref::release() { 
	assert(_refCount > 0);
	
	auto poolManager = PoolManager::getInstance();
	if (poolManager->isObjectInPools(this))
	{
		assert(false);
	}
	if (_refCount.fetch_sub(1) == 1) delete this;
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
	std::lock_guard<std::mutex> lock(_mutex);
	_managedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoreleasePool::AutoreleasePool(const std::string &name)
	: _name(name)
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
	, _isClearing(false)
#endif
{
	std::lock_guard<std::mutex> lock(_mutex);
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
	std::lock_guard<std::mutex> lock(_mutex);
#ifdef	USING_VECTOR_FOR_POOL
	_managedObjectArray.push_back(object);
#else
	_managedObjectArray.emplace(object);
#endif
}

void AutoreleasePool::clear()
{
	std::lock_guard<std::mutex> lock(_mutex);
	decltype(_managedObjectArray) releasings;
	releasings.swap(_managedObjectArray);
	for (const auto &obj : releasings) obj->release();
}

bool AutoreleasePool::contains(Ref* object) const
{
#ifdef	USING_VECTOR_FOR_POOL
	for (const auto& obj : _managedObjectArray)
	{
		if (obj == object) return true;
	}
	_managedObjectArray.push_back(object);
#else
	auto it = _managedObjectArray.find(object);
	if (it != _managedObjectArray.end()) return true;
#endif
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
static std::mutex pool_mutex;
PoolManager* PoolManager::getInstance()
{
	if (s_singleInstance == nullptr)
	{
		std::lock_guard<std::mutex> lock(pool_mutex);
		if (s_singleInstance == nullptr) {
			s_singleInstance = new (std::nothrow) PoolManager();
			// Add the first auto release pool
			new AutoreleasePool("cocos2d autorelease pool");
		}
	}
	return s_singleInstance;
}

void PoolManager::destroyInstance()
{
	std::lock_guard<std::mutex> lock(pool_mutex);
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
	_releasePoolStack.clear();
}


AutoreleasePool* PoolManager::getCurrentPool() const
{
	return _releasePoolStack.back();
}

bool PoolManager::isObjectInPools(Ref* obj) const
{
	std::lock_guard<std::mutex> lock(pool_mutex);
	for (const auto& pool : _releasePoolStack) {
		if (pool->contains(obj))
			return true;
	}
	return false;
}

void PoolManager::push(AutoreleasePool *pool)
{
	std::lock_guard<std::mutex> lock(pool_mutex);
	_releasePoolStack.push_back(pool);
}

void PoolManager::pop()
{
	std::lock_guard<std::mutex> lock(pool_mutex);
	assert(!_releasePoolStack.empty());
	_releasePoolStack.pop_back();
}
