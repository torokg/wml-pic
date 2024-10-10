#ifndef TYPES_META_BIND_HH
# define TYPES_META_BIND_HH
#include <functional>

namespace types::meta
{

/*! type_list is a dependency */
template<typename...>
struct type_list;

/*! Provides template-indexable placeholder class.
 *  Unlike std::placeholders::_*, these placeholder
 *  indices are zero-based */
template<size_t i>
struct ph : std::integral_constant<size_t,1+i> {};

}
#ifdef ANDROID
namespace std
{

inline namespace __ndk1
{
template<typename T>
struct is_placeholder;

template<size_t i>
struct is_placeholder<::types::meta::ph<i>> : std::integral_constant<int, static_cast<int>(i+1)> {};

template<typename T>
struct is_placeholder<::types::meta::unfold<T>> : std::integral_constant<int, 0> {};

}

}
#else
namespace std
{

template<typename T>
struct is_placeholder;

template<size_t i>
struct is_placeholder<::types::meta::ph<i>> : std::integral_constant<int, static_cast<int>(i+1)> {};

template<typename T>
struct is_placeholder<::types::meta::unfold<T>> : std::integral_constant<int, 0> {};

}
#endif

namespace types::meta
{

namespace _detail::_bind
{

template<typename Tl, typename Te>
struct unfolder
{
	typedef typename Tl
		::template prepend<Te>
	list;
};

template<typename Tl, template<typename...> typename Tpt, typename... Ts>
struct unfolder<Tl,unfold<Tpt<Ts...>>>
{
	typedef typename Tl
		::template prepend<Ts...>
	list;
};

template<typename Tx, typename TBindArgs, typename TResultArgs>
struct binder;

template<typename Tb, typename Tr, typename... TBindArgs, typename... TResultArgs>
struct binder<
	std::enable_if_t<std::is_placeholder_v<Tb>>,
	type_list<Tb,TBindArgs...>,
	type_list<Tr,TResultArgs...>
>
{
	typedef typename unfolder<
		typename binder<void,type_list<TBindArgs...>,type_list<TResultArgs...>>::list
	,
		Tr
	>::list list;
};

template<typename Tb, typename... TBindArgs, typename... TResultArgs>
struct binder<
	std::enable_if_t<!std::is_placeholder_v<Tb>>,
	type_list<Tb,TBindArgs...>,
	type_list<TResultArgs...>
>
{
	typedef typename unfolder<
		typename binder<void,type_list<TBindArgs...>,type_list<TResultArgs...>>::list
	,
		Tb
	>::list list;
};

template<typename Tr, typename... TResultArgs>
struct binder<
	void,
	type_list<>,
	type_list<Tr,TResultArgs...>
>
{
	typedef typename unfolder<
		typename binder<void,type_list<>,type_list<TResultArgs...>>::list
	,
		Tr
	>::list list;
};

template<typename Tx>
struct binder<Tx,type_list<>,type_list<>>
{ typedef type_list<> list; };

}

/*! Binds the given arguments to the first corresponding number of template arguments of T,
 *  making it possible to use T as a template-template parameter with different template 
 *  signature. (Just the same as that std::bind does to functions) */
template<template<typename...> typename T, typename... TBindArgs>
struct bind
{
	template<typename... TResultArgs>
	using type = typename _detail::_bind::binder<void, type_list<TBindArgs...>, type_list<TResultArgs...>>
		::list
		::template apply<T>
	;
};

}
#endif
