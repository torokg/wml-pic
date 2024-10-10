#ifndef TYPES_META_ANNOTATION_HH
# define TYPES_META_ANNOTATION_HH
# include <utility>
namespace types::meta
{

/*! Indicates that T's template arguments shall be taken instead of T itself */
template<typename T>
struct unfold;

template<template<typename...> typename T, typename... Ts>
class unfold<T<Ts...>>
{
	T<Ts...>&& _value;
public:
	constexpr inline unfold(T<Ts...>&& v)
		: _value(std::forward<T<Ts...>>(v))
	{}

	constexpr inline operator T<Ts...>()
	{ return std::forward<T>(_value); }
};


template<typename T>
struct verbose
{
	T&& _value;
public:
	constexpr inline verbose(T&& v)
		: _value(std::forward<T>(v))
	{}

	constexpr inline operator T() const
	{ return std::forward<T>(_value); }

	constexpr inline T &&forward() const
	{ return std::forward<T>(_value); }

	constexpr inline T &&move() const
	{ return std::move(_value); }
};

template<typename T>
verbose(T&& v) -> verbose<T>;

}

#endif
