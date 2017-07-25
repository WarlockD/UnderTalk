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
#include <atomic>


#include <flyweight/object.hpp>
#include <flyweight/cache.hpp>
#include <string>

class istring {
public:
	using string_type = std::string;
	using cache_type = flyweight::cache<string_type>;

	using value_type = std::add_const_t<string_type>;
	using key_type = typename cache_type::key_type;

	using reference = std::add_lvalue_reference_t<value_type>;
	using pointer = std::add_pointer_t<value_type>;
	// intresting
	template <
		class... Args,
		class = std::enable_if_t<
			std::conjunction<
				std::negation<
					std::is_same<std::decay_t<Args>, istring>...>,
					std::is_constructible<value_type, Args...>
				>::value
		>
		> istring(Args&&... args) : istring{
		value_type{ std::forward<Args>(args)... }
	} { }

	template <
		class ValueType,
		class = std::enable_if_t<
			std::is_same<std::decay_t<ValueType>, std::decay_t<value_type>
		>::value
		>
	> explicit istring(ValueType&& value) : handle{ cache_type::ref().find(std::forward<ValueType>(value)) } { }

	istring(istring const&) = default;
	istring(istring&&) = delete;
	istring() : istring { value_type{} } { }
	~istring() = default;

	istring& operator = (istring const&) = default;
	istring& operator = (istring&&) = delete;

	void swap(istring& that) noexcept {
		using std::swap;
		swap(this->handle, that.handle);
	}
	template <
		class ValueType,
		class = std::enable_if_t<
			!std::is_same<object, std::decay_t<ValueType>>::value
		>> istring& operator = (ValueType&& value) {
		handle = cache_type::ref().find(::std::forward<ValueType>(value));
		//object tmp(std::forward<ValueType>(value));
		//tmp.swap(*this);
		return *this;
	}
	size_t size() const { return handle->size(); }
	auto begin() const { return handle->begin(); }
	auto end() const { return handle->end(); }
	bool operator==(const string_type& s) const { return *handle == s; }
	bool operator!=(const string_type& s) const { return *handle != s; }
	bool operator==(const istring& s) const { return handle.get() == s.handle.get(); }
	bool operator!=(const istring& s) const { return handle.get() != s.handle.get(); }
	const char* c_str() const { return handle->c_str(); }
	pointer operator -> () const noexcept { return this->handle.get(); }
	operator reference () const noexcept { return *this->handle.get(); }
private:
	std::shared_ptr<value_type> handle;
};


namespace std {
	template<>
	struct hash<istring> {
		size_t operator ()(istring const& value) const noexcept {
			return std::hash<istring::string_type>()(value);
		}
	};
};



class Ref  {
	std::atomic<size_t> _refCount;
public:
	unsigned int Ref::getReferenceCount() const { return _refCount.load(); }
	Ref() :_refCount(1) {}
	virtual ~Ref() {  }
	void retain();
	void release();
	Ref* autorelease();
};
// basic object



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

using String = flyweight::object<std::string>;  // simple internned string

template<typename C, typename E, typename T>
typename std::enable_if<priv::has_left_shift<std::basic_ostream<C,E>,T>::value, std::basic_ostream<C, E>&>::type
static  inline operator <<(std::basic_ostream<C,E>& os, const flyweight::object<T>& value)  {
	os << value.get();
	return os;
}

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
#ifdef USING_VECTOR_FOR_POOL
	std::vector<Ref*> _managedObjectArray;
#else
	std::unordered_set<Ref*>  _managedObjectArray;
#endif
	std::string _name;
	std::mutex _mutex;
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