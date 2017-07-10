#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <flyweight/object.hpp>
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
// basic object

template<typename T>
class LinkedNode {
public:
	using type = std::remove_const_t<T>;
	using node_type = LinkedNode<T>;
	using pointer = T*;
	using refrence = T&;
	using field_type = LinkedNode<T> T::*;
private:
	T* _head;
	T* _next;
	T** _prev;

	template<field_type field>
	struct list_helper {
		constexpr static size_t field_prev_offset() { return  (size_t) &((reinterpret_cast<T*>(0)->*field)._prev); }

		template<class M> constexpr static T* container_of(M* ptr) { return (T*)((char*)ptr - field_offset); }
		static_assert(field != nullptr, "Don't know what field you want!");
		static inline pointer& next(pointer elm) { return (elm->*field)._next; }
		static inline pointer& parent(pointer elm) { return (elm->*field)._head; }
		static inline pointer prev(pointer elm) { // hurrm, TEST PREV
			return ((elm->*field)._prev == &parent(elm) ? nullptr : elm->*field)._prev;
			//if ((elm->*field)._prev == &parent(elm)) return nullptr;
			//return reinterpret_cast<pointer>(reinterpret_cast<char*>(&((elm->*field)._prev)) - field_prev_offset);
		}
		static inline bool is_linked(pointer elm) { return (elm->*field)._head != nullptr; }
		static inline void remove(pointer elm) {
			if (next(elm) != nullptr)
				(next(elm)->*field)._prev = (elm->*field)._prev;
			*(elm->*field)._prev = next(elm);
			(elm->*field)._head = nullptr;
		}
		static inline void insert_after(pointer listelm, pointer elm) {
			if (is_linked(elm)) remove(elm);
			if ((next(elm) = next(listelm)) != nullptr)
				(next(listelm)->*field)._prev = &next(elm);
			next(listelm) = elm;
			(elm->*field)._prev = &_list_next(listelm);
			(elm->*field)._head = (elm->*listelm)._head;
		}
		static inline void insert_before(pointer listelm, pointer elm) {
			if (is_linked(elm)) remove(elm);
			(elm->*field)._prev = (listelm->*field)._prev;
			next(elm) = listelm;
			*(listelm->*field)._prev = elm;
			(listelm->*field)._prev = &next(elm);
			(elm->*field)._head = (elm->*listelm)._head;
		}
		static inline void insert_head(pointer& head, pointer elm) {
			if (is_linked(elm)) remove(elm);
			if ((next(elm) = head) != nullptr)
				(head->*field)._prev = &next(elm);
			head = elm;
			(elm->*field)._prev = &head;
			(elm->*field)._head = head;
		}
	};

public:
	bool linked() const { return _head != nullptr; }

	template<field_type field, bool _is_const>
	struct iterator_impl {
		using list = list_helper<field>;
		pointer _current;
		inline pointer _next() const { return _current ? list::next(_current) : nullptr; }
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = typename std::conditional<_is_const, const  T, T>::type;
		using pointer = value_type*;
		using reference = value_type&;
		//using iterator_category = std::bidirectional_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using iterator = iterator_impl<field,_is_const>;
		// Must downcast from _List_node_base to _List_node to get to _M_data.
		value_type& operator*()  const noexcept { return *_current; }
		//const_value_type& operator*()  noexcept const { return *_current; }
		value_type& operator&()  const noexcept { return *_current; }
		pointer operator->()  const noexcept { return _current; }

		// do I need to test if the field pointers are equal?
		bool operator==(const iterator_impl& r) const { return _current == r._current; }
		bool operator!=(const iterator_impl& r) const { return _current != r._current; }
		iterator_impl(pointer current) : _current(current) {}

		inline iterator_impl operator++() noexcept { _current = _next(); return iterator(*this); }
		inline iterator_impl operator++(int) noexcept { iterator tmp(*this); _current = _next(); return tmp; }
	};
	template<field_type field> using iterator = iterator_impl<field, false>;
	template<field_type field> using const_iterator = iterator_impl<field, true>;

	LinkedNode() : _head(nullptr) , _next(UNLINK_MAGIC), _prev(&next) {}
	T const * parent() const { return _head; }
	// skip this as we have to cacuate the field offset
#if 0
	template<field_type field>
	inline void remove() { if(_head != nullptr) list_helper<field>::remove(this); }
#else
	template<field_type field>
	inline void remove(pointer ptr) { if (list_helper<field>::parent(ptr)  != nullptr) list_helper<field>::remove(ptr); }
#endif
	template<field_type field>
	inline void insert_head(pointer& head) { list_helper<field>::insert_head(head, this); }
	template<field_type field>
	inline void insert_after(pointer listelm) { list_helper<field>::insert_after(listelm, this); }
	template<field_type field>
	inline void insert_before(pointer listelm) { list_helper<field>::insert_before(listelm, this); }

	// wrapper for the list
	template<field_type field>
	class LinkedListWrapper {
		pointer& _head;
	public:
		using iterator = iterator_impl<field, false>;
		using const_iterator = iterator_impl<field, true>;

		LinkedListWrapper(pointer& head) : _head(head) {}
		inline  iterator begin() { return iterator(_head); }
		inline  iterator end() { return  iterator(nullptr); }
		inline  const_iterator begin() { return  const_iterator(_head); }
		inline  const_iterator end() { return  const_iterator(nullptr); }
		bool empty() const { return _head == nullptr; }
	};
	// wrapper for the list
	template<field_type field>
	class LinkedList {
		using list = list_helper<field>;
		pointer _head;
	public:
		using iterator = iterator_impl<field, false>;
		using const_iterator = iterator_impl<field, true>;

		LinkedList() : _head(nullptr) {}
		inline  iterator begin() { return iterator(_head); }
		inline  iterator end() { return  iterator(nullptr); }
		inline  const_iterator begin() const { return  const_iterator(_head); }
		inline  const_iterator end() const { return  const_iterator(nullptr); }
		void push_front(pointer p) { list::insert_head(_head, p); }
		void remove(pointer p) { 
			assert(list::parent(p) == _head);
			if (list::parent(p) == _head) 
				list::remove(p);
		}
		bool empty() const { return _head == nullptr; }
	};
	template<field_type field> inline  iterator<field> begin() { return iterator<field>(_head); }
	template<field_type field> inline  iterator<field> end() { return  iterator<field>(nullptr); }
	template<field_type field> inline  const_iterator<field> begin() { return  const_iterator<field>(_head); }
	template<field_type field> inline  const_iterator<field> end() { return  const_iterator<field>(nullptr); }
};

// below is if we wanted object to always be unique
#if 0
class Object {
	static std::vector<Object const*>& all_objects() {
		static std::vector<Object const*> _objects;
		return _objects;
	}
	int _mark=0;
	LinkedNode<Object> _link;
	LinkedNode<Object>::LinkedList<&Object::_link> _children;
protected:
	void addObject(Object& obj) {
		assert(&obj != this); // sanity
		_children.remove(&obj);
	}
	void removeSelf() {
		_link.remove<&Object::_link>(this);
	}
	void set_parent(Object const* p) { }
	bool has_parent() const { return _link.parent() != nullptr; }
	Object const * parent() const { return _link.parent(); }
public:
	virtual bool equals(const Object& other) const { return this == &other; }
	virtual size_t hash() const { return std::hash<const void*>()(static_cast<const void*>(this)); }
	virtual ~Object() {}
};
#endif

namespace priv {
	template<class T, class EqualTo>
	struct has_operator_equal_impl
	{
		template<class U, class V>
		static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
		template<typename, typename>
		static auto test(...)->std::false_type;

		using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
	};

	template<class T, class EqualTo = T>
	struct has_operator_equal : has_operator_equal_impl<T, EqualTo>::type {};
	//https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
	// intresting about c++14
	// As of c++14, the standard binary functions do most of the work for us for the majority of operators.

	template<typename X, typename Y, typename Op>
	struct op2_valid_impl{ // binary op
		template<typename U, typename L, typename R>
		static auto test(int) -> decltype(std::declval<U>()(std::declval<L>(), std::declval<R>()),
			void(), std::true_type());
		template<typename U, typename L, typename R>
		static auto test(...)->std::false_type;
		using type = decltype(test<Op, X, Y>(0));
	};
	template<typename X, typename Op>
	struct op1_valid_impl{ // single op
		template<typename U, typename L>
		static auto test(int) -> decltype(std::declval<U>()(std::declval<L>()),
			void(), std::true_type());
		template<typename U, typename L>
		static auto test(...)->std::false_type;
		using type = decltype(test<Op, X>(0));
	};

	template<typename X, typename Y, typename Op> using op2_valid = typename op2_valid_impl<X, Y, Op>::type;
	template<typename X, typename Op> using op1_valid = typename op1_valid_impl<X, Op>::type;



	namespace notstd {
		struct left_shift {

			template <class L, class R>
			constexpr auto operator()(L&& l, R&& r) const
				noexcept(noexcept(std::forward<L>(l) << std::forward<R>(r)))
				-> decltype(std::forward<L>(l) << std::forward<R>(r))
			{
				return std::forward<L>(l) << std::forward<R>(r);
			}
		};

		struct right_shift {

			template <class L, class R>
			constexpr auto operator()(L&& l, R&& r) const
				noexcept(noexcept(std::forward<L>(l) >> std::forward<R>(r)))
				-> decltype(std::forward<L>(l) >> std::forward<R>(r))
			{
				return std::forward<L>(l) >> std::forward<R>(r);
			}
		};

	}

	template<typename X, typename Y> using has_equality = op2_valid<X, Y, std::equal_to<>>;
	template<typename X, typename Y> using has_inequality = op2_valid<X, Y, std::not_equal_to<>>;
	template<typename X, typename Y> using has_less_than = op2_valid<X, Y, std::less<>>;
	template<typename X, typename Y> using has_less_equal = op2_valid<X, Y, std::less_equal<>>;
	template<typename X, typename Y> using has_greater_than = op2_valid<X, Y, std::greater<>>;
	template<typename X, typename Y> using has_greater_equal = op2_valid<X, Y, std::greater_equal<>>;
	template<typename X, typename Y> using has_bit_xor = op2_valid<X, Y, std::bit_xor<>>;
	template<typename X, typename Y> using has_bit_or = op2_valid<X, Y, std::bit_or<>>;
	template<typename X, typename Y> using has_left_shift = op2_valid<X, Y, notstd::left_shift>;
	template<typename X, typename Y> using has_right_shift = op2_valid<X, Y, notstd::right_shift>;
	template<typename X> using has_hash = op1_valid<X, std::hash<X>>;
	
};
template<typename T>
class ObjectPool {
	using value_type = std::decay_t<T>;
	using pointer = value_type*;
	using refrence = value_type&;
	using const_pointer = const pointer;
	using const_refrence = const refrence;
	using unique_ptr = std::shared_ptr<T>;
	using unique_ref = std::reference_wrapper<const T>;
	struct unique_equal {
		bool operator(const unique_ref& l, const unique_ref& r) const noexcept{ return l.get() == r.get(); }
	};
	struct unique_hash {
		size_t operator(const unique_ref& l) const noexcept { return std::hash<T>()(l.get());  }
	};
	using unique_object_map = std::unordered_map<unique_ref,unique_ptr, unique_hash, unique_equal>;
	unique_object_map _pool;
	static unique_object_map& all_objects() {
		static unique_object_map _objects;
		return _objects;
	}


};

class Object {
	using object_pointer_set = std::unordered_set<Object*>;
	static object_pointer_set& all_objects() {
		static object_pointer_set _objects;
		return _objects;
	}
	int _mark = 0;
	object_pointer_set _objects;
	Object* _parent;
	friend class ObjectRef;

protected:
	Object() :_mark(0) { all_objects().emplace(this); }
	virtual ~Object() {  }
public:
	virtual bool equals(const Object& other) const { return this == &other; }
	virtual size_t hash() const { return std::hash<Object const*>()(this); }
	static void collect_garbage(Object* root) {
		root->mark(); // mark eveything from root
		object_pointer_set& garbage = all_objects(); 
		for (auto it = garbage.begin(); it != garbage.end(); ) {
			Object * obj = (*it);
			if (obj->_mark == 1) {
				it = garbage.erase(it);
				delete obj;
			}
			else {
				obj->_mark = 0;
				++it;
			}
		}
	}
	Object* parent() { return _parent; }
	const Object* parent() const { return _parent; }
	void addObject(Object* p) {
		assert(p != this);
		_objects.emplace(p);
		p->_parent = this;
	}
	void removeSelf() {
		if (_parent != nullptr) {
			_parent->_objects.erase(this);
			_parent = nullptr;
		}
	}
	void removeObject(Object* p) {
		assert(p->_parent == this);
		if (p->_parent == this) p->removeSelf();
	}
	virtual int mark()  {
		int tmp = _mark;
		if (tmp == 0) {
			_mark = 2; // just for tracing but we ar marked gray during trasverse
			for (auto o : _objects) o->mark();
			_mark = 1; // mark peroperly that we are done
			return 1;
		}
		return 0;
	}
};
namespace std {
	template<>
	struct hash<Object> {
		size_t operator()(const Object& obj) const {
			return obj.hash();
		}
	};
};


template<typename L, typename R>
//typename std::enable_if<std::is_base_of<Object,std::decay_t<T>>::value && std::is_base_of<Object, std::decay_t<F>>::value,bool>::type
static inline auto operator==(const L& l, const R&r) -> decltype(l.equals(r)) {
	return l.equals(r);
};

template<typename T>
class ObjectWraper : public Object {
	T _obj;
	struct use_object_func {};
	struct use_std_func : use_object_func {};
	struct use_member_func : use_std_func {};
	template<typename U>
	inline size_t hash_impl(U const& r, use_object_func) const { return Object::hash(); }
	template<typename U>
	inline auto hash_impl(U const& r, use_std_func) const -> decltype(std::hash<T>()(T{})) { return std::hash<T>()(r); }
	template<typename U>
	inline auto hash_impl(U const& r, use_member_func) const -> decltype(T().hash()) { return r.hash(); }
	template<typename U>
	inline bool equals_impl(U const& r, use_object_func) const { return Object::equals(r); }
	template<typename U>
	inline auto equals_impl(U const& r, use_std_func) const -> decltype(std::equal_to<U>()(T(),r)) { return Object::equals(r) || std::equal_to<U>()(*_obj, r); }
	template<typename U>
	inline auto equals_impl(U const& r, use_member_func) const -> decltype(T().equals(r)) { return Object::equals(r) || _obj.equals(r); }

	inline bool equals_impl(Object const& r, use_object_func) const { return Object::equals(r); }
public:
	using value_type = T;
	using pointer = std::add_pointer_t<T>;
	using refrence = std::add_lvalue_reference<T>;
	using const_pointer = const pointer;
	using const_refrence = const refrence;
	template<typename ... Args>
	ObjectWraper(Args ... args) : _obj(std::forward<Args>(args)...) : Object() {}
	// Must downcast from _List_node_base to _List_node to get to _M_data.
	const_refrence operator*()  const noexcept { return _current; }
	const_refrence operator&()  const noexcept { return _obj; }
	const_pointer operator->()  const noexcept { return &_obj; }
	refrence operator*()   noexcept { return _current; }
	refrence operator&()   noexcept { return _obj; }
	pointer operator->()   noexcept { return &_obj; }
	virtual size_t hash() const { 
		return hash_impl(_obj, use_member_func{});
#if 0
		return hash_impl(_obj, 

		std::conditional_t<std::is_same<decltype(_obj.hash()), size_t>::value, use_member_func{}, 
		std::conditional_t<std::is_same<decltype(std::hash<T>()(_obj)), size_t>::value, use_std_func, use_object_func>>{});
#endif
	}
//	virtual bool equals(const Object& o) const { return equals_impl<T>(o, use_member_func{}); }
	virtual bool equals(const T& o) const { return equals_impl<T>(o, use_member_func{}); }
};

class ObjectString : public Object {
	std::string _str;
public:

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