#ifndef SCHED_LOCKABLE_H
# define SCHED_LOCKABLE_H
# include <shared_mutex>
namespace sched::_detail::lockable
{

template<typename T, typename Tmutex, typename = void>
struct lockable;

template<typename T, typename Tmutex>
struct lockable<
	T, Tmutex,
	std::enable_if_t<
		std::is_class_v<T> ||
		std::is_union_v<T>
	>
>
	: T
	, Tmutex
{
	template<typename... Targs>
	lockable(Targs&&... args)
		: T(std::forward<Targs>(args)...)
	{}
};

template<typename T, typename Tmutex>
class lockable<T*,Tmutex,void>
	: public Tmutex
{
	T *value;

public:
	lockable() = default;

	inline lockable(T* v)
		: value(v)
	{}

	inline T* &operator=(T *v)
	{
		value = v;
		return value;
	}

	inline operator T*&()
	{ return value; }

	inline operator T* const&() const
	{ return value; }

	inline T &operator*() const
	{ return *value; }

	inline T *operator->() const
	{ return value; }

};

template<typename T, typename Tmutex>
class lockable<
	T,Tmutex,
	std::enable_if_t<
		std::is_integral_v<T> ||
		std::is_floating_point_v<T>
	>
>
	: public Tmutex
{
	T value;

public:

	lockable() = default;

	inline lockable(T v)
		: value(v)
	{}

	inline operator T&()
	{ return value; }

	inline operator const T&() const
	{ return value; }

	template<typename U>
	inline auto operator+(U &&u) const
	{ return value+std::forward<U>(u); }

	template<typename U>
	inline auto operator-(U &&u) const
	{ return value-std::forward<U>(u); }

	template<typename U>
	inline auto operator*(U &&u) const
	{ return value*std::forward<U>(u); }

	template<typename U>
	inline auto operator/(U &&u) const
	{ return value/std::forward<U>(u); }

	template<typename U>
	inline auto operator%(U &&u) const
	{ return value%std::forward<U>(u); }

	template<typename U>
	inline auto operator|(U &&u) const
	{ return value|std::forward<U>(u); }

	template<typename U>
	inline auto operator&(U &&u) const
	{ return value&std::forward<U>(u); }

	template<typename U>
	inline auto operator^(U &&u) const
	{ return value^std::forward<U>(u); }


	template<typename U>
	inline auto operator=(U &&u)
	{ return value = std::forward<U>(u); }

	template<typename U>
	inline auto operator+=(U &&u)
	{ return value+=std::forward<U>(u); }

	template<typename U>
	inline auto operator-=(U &&u)
	{ return value-=std::forward<U>(u); }

	template<typename U>
	inline auto operator*=(U &&u)
	{ return value*=std::forward<U>(u); }

	template<typename U>
	inline auto operator/=(U &&u)
	{ return value/=std::forward<U>(u); }

	template<typename U>
	inline auto operator%=(U &&u)
	{ return value%=std::forward<U>(u); }

	template<typename U>
	inline auto operator|=(U &&u)
	{ return value|=std::forward<U>(u); }

	template<typename U>
	inline auto operator&=(U &&u)
	{ return value&=std::forward<U>(u); }

	template<typename U>
	inline auto operator^=(U &&u)
	{ return value^=std::forward<U>(u); }
};

}

namespace sched
{

template<typename T, typename Tmutex = std::shared_mutex>
using lockable = typename _detail::lockable::lockable<T,Tmutex>;

}
#endif
