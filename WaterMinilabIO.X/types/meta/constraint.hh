#ifndef TYPES_META_CONSTRAINT_HH
# define TYPES_META_CONSTRAINT_HH
# include <type_traits>
# include <types/meta/swallow.hh>

namespace types::meta
{

namespace _detail
{
	template<typename T, typename E = void>
	struct is_container : std::false_type {};

	template<typename T>
	struct is_container<
		T,
		std::void_t<
			typename T::value_type,
			typename T::size_type,
			typename T::iterator,
			decltype(std::declval<T>().size()),
			decltype(std::declval<T>().begin()),
			decltype(std::declval<T>().end())
		>
	> : std::true_type {};


	template<typename T, typename E = void>
	struct is_iterable: std::false_type {};

	template<typename T>
	struct is_iterable<
		T,
		std::void_t<
			decltype(std::declval<T>().begin()),
			decltype(std::declval<T>().end()),
			decltype(std::declval<decltype(std::declval<T>().begin())>()++)
		>
	> : std::true_type {};

	template<typename T, typename E = void>
	struct is_emplacable: std::false_type {};

	template<typename T>
	struct is_emplacable<
		T,
		std::void_t<decltype(std::declval<T>().emplace(std::declval<T>().end(), std::declval<typename T::value_type>()))>
	> : std::true_type {};

	template<typename T, typename E = void>
	struct is_back_emplacable: std::false_type {};

	template<typename T>
	struct is_back_emplacable<
		T,
		std::void_t<decltype(std::declval<T>().emplace_back(std::declval<typename T::value_type>()))>
	> : std::true_type {};


	template<typename T, typename E = void>
	struct is_front_emplacable: std::false_type {};

	template<typename T>
	struct is_front_emplacable<
		T,
		std::void_t<decltype(std::declval<T>().emplace_front(std::declval<typename T::value_type>()))>
	> : std::true_type {};

	template<typename T, typename E = void>
	struct is_insertable: std::false_type {};

	template<typename T>
	struct is_insertable<
		T,
		std::void_t<decltype(std::declval<T>().insert(std::declval<T>().end(), std::declval<typename T::value_type>()))>
	> : std::true_type {};

	template<typename T, typename E, typename... Ts>
	struct is_callable : std::false_type {};

	template<typename T,typename... Ts>
	struct is_callable<
		T,
		std::void_t<decltype(std::declval<T>()(std::declval<Ts>()...))>,
		Ts...
	> : std::true_type {};

	template<typename T, typename E, typename... Ts>
	struct is_constructible_from : std::false_type {};

	template<typename T, typename... Ts>
	struct is_constructible_from<
		T,
		std::void_t<decltype(T(std::declval<Ts>()...))>,
		Ts...
	> : std::true_type {};
}

template<typename T, typename... Ts>
struct is_constructible_from : _detail::is_constructible_from<T,void,Ts...> {};

template<typename T, typename... Ts>
constexpr bool is_constructible_from_v = is_constructible_from<T,Ts...>::value;

template<typename T, typename U>
using is_same = typename std::is_same<T,U>;

template<template<typename...> typename Ta, template<typename...> typename Tb>
struct is_same_template : std::false_type {};

template<template<typename...> typename T>
struct is_same_template<T,T> : std::true_type {};

template<template<typename...> typename Ta, template<typename...> typename Tb>
constexpr bool is_same_template_v = is_same_template<Ta,Tb>::value;


template<typename T, template<typename...> typename Ttmpl>
struct is_instance_of : std::false_type {};

template<template<typename...> typename Ttmpl, typename... Targs>
struct is_instance_of<Ttmpl<Targs...>,Ttmpl> : std::true_type {};

template<typename T, template<typename...> typename Ttmpl>
constexpr bool is_instance_of_v = is_instance_of<T,Ttmpl>::value;


template<typename T, typename... Tn>
struct is_one_of : std::false_type {};

template<typename T1, typename T2, typename... Tn>
struct is_one_of<T1,T2,Tn...> : std::bool_constant
<
	std::is_same_v<T1,T2> ||
	is_one_of<T1,Tn...>::value
> {};

template<typename T1, typename T2>
struct is_one_of<T1,T2> : std::is_same<T1,T2> {};

template<typename... Targs>
constexpr bool is_one_of_v = is_one_of<Targs...>::value;


template <typename Tmain, typename T, typename... Targs>
struct is_all_of : std::bool_constant
<
	std::is_same_v<Tmain, T> &&
	is_all_of<Tmain, Targs...>::value
>
{};

template <typename Tmain, typename T>
struct is_all_of<Tmain, T> : std::is_same<Tmain, T>
{};

template <typename... Targs>
constexpr bool is_all_of_v = is_all_of<Targs...>::value;


template<typename T>
struct is_container : _detail::is_container<T> {};

template<typename T>
constexpr bool is_container_v = is_container<T>::value;


template<typename T>
struct is_iterable : _detail::is_iterable<T> {};

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;


template<typename T>
struct is_emplacable: _detail::is_emplacable<T> {};

template<typename T>
constexpr bool is_emplacable_v = is_emplacable<T>::value;


template<typename T>
struct is_back_emplacable: _detail::is_back_emplacable<T> {};

template<typename T>
constexpr bool is_back_emplacable_v = is_back_emplacable<T>::value;


template<typename T>
struct is_front_emplacable: _detail::is_front_emplacable<T> {};

template<typename T>
constexpr bool is_front_emplacable_v = is_front_emplacable<T>::value;


template<typename T>
struct is_insertable: _detail::is_insertable<T> {};

template<typename T>
constexpr bool is_insertable_v = is_insertable<T>::value;


template<typename T,typename... Ts>
struct is_callable: _detail::is_callable<T,void,Ts...> {};

template<typename T>
constexpr bool is_callable_v = is_callable<T>::value;


template<typename T>
using is_nullary_callable = is_callable<T>;

template<typename T>
constexpr bool is_nullary_callable_v = is_nullary_callable<T>::value;


template<typename Tb, typename Tc, typename... Targs>
struct is_base_of_any;

template<typename Tb, typename Tc, typename Td, typename... Targs>
struct is_base_of_any<Tb,Tc,Td,Targs...> : std::bool_constant<std::is_base_of_v<Tb,Tc> || is_base_of_any<Tb,Td,Targs...>::value> {};

template<typename Tb,typename Tc>
struct is_base_of_any<Tb,Tc> : std::bool_constant<std::is_base_of_v<Tb,Tc>> {};

template<typename... Ts>
constexpr bool is_base_of_any_v = is_base_of_any<Ts...>::value;


template<typename Tb, typename Tc, typename... Targs>
struct is_base_of_all;

template<typename Tb, typename Tc, typename Td, typename... Targs>
struct is_base_of_all<Tb,Tc,Td,Targs...> : std::bool_constant<std::is_base_of_v<Tb,Tc> && is_base_of_all<Tb,Td,Targs...>::value> {};

template<typename Tb,typename Tc>
struct is_base_of_all<Tb,Tc> : std::bool_constant<std::is_base_of_v<Tb,Tc>> {};

template<typename... Ts>
constexpr bool is_base_of_all_v = is_base_of_all<Ts...>::value;


template<typename Ta, typename Tb>
struct is_child_of : std::bool_constant<std::is_base_of_v<Tb,Ta>> {};

template<typename Ta, typename Tb>
constexpr bool is_child_of_v = is_child_of<Ta,Tb>::value;


template<typename Tb, typename Tc, typename... Targs>
struct is_child_of_any;

template<typename Tb, typename Tc, typename Td, typename... Targs>
struct is_child_of_any<Tb,Tc,Td,Targs...> : std::bool_constant<is_child_of_v<Tb,Tc> || is_child_of_any<Tb,Td,Targs...>::value> {};

template<typename Tb,typename Tc>
struct is_child_of_any<Tb,Tc> : std::bool_constant<is_child_of_v<Tb,Tc>> {};

template<typename... Ts>
constexpr bool is_child_of_any_v = is_child_of_any<Ts...>::value;


template<typename Tb, typename Tc, typename... Targs>
struct is_child_of_all;

template<typename Tb, typename Tc, typename Td, typename... Targs>
struct is_child_of_all<Tb,Tc,Td,Targs...> : std::bool_constant<is_child_of_v<Tb,Tc> && is_child_of_all<Tb,Td,Targs...>::value> {};

template<typename Tb,typename Tc>
struct is_child_of_all<Tb,Tc> : std::bool_constant<is_child_of_v<Tb,Tc>> {};

template<typename... Ts>
constexpr bool is_child_of_all_v = is_child_of_all<Ts...>::value;


template<typename Ta, typename Tb>
struct is_relative_of : std::bool_constant<std::is_base_of_v<Ta,Tb> || is_child_of_v<Ta,Tb>> {};

template<typename Ta, typename Tb>
constexpr bool is_relative_of_v = is_relative_of<Ta,Tb>::value;

template<typename Tb, typename Tc, typename... Targs>
struct is_relative_of_any : std::bool_constant<is_base_of_any_v<Tb,Tc,Targs...>||is_child_of_any_v<Tb,Tc,Targs...>> {};

template<typename... Ts>
constexpr bool is_relative_of_any_v = is_relative_of_any<Ts...>::value;


template<typename Tb, typename Tc, typename... Targs>
struct is_relative_of_all;

template<typename Tb, typename Tc, typename Td, typename... Targs>
struct is_relative_of_all<Tb,Tc,Td,Targs...> : std::bool_constant<is_relative_of_v<Tb,Tc> && is_relative_of_all<Tb,Td,Targs...>::value> {};

template<typename Tb,typename Tc>
struct is_relative_of_all<Tb,Tc> : std::bool_constant<is_relative_of_v<Tb,Tc>> {};

template<typename... Ts>
constexpr bool is_relative_of_all_v = is_relative_of_all<Ts...>::value;


template<typename T, typename... Ts>
struct are_distinct
	: std::bool_constant<!is_one_of_v<T,Ts...> && are_distinct<Ts...>::value>
{};

template<typename T>
struct are_distinct<T> : std::true_type {};

template<typename T, typename... Ts>
constexpr bool are_distinct_v = are_distinct<T,Ts...>::value;

template<typename T, typename TT, typename... Targs>
struct is_convertible_all : std::bool_constant<std::is_convertible_v<T,TT> && is_convertible_all<T,Targs...>::value> {};

template<typename T, typename TT>
struct is_convertible_all<T,TT> : std::bool_constant<std::is_convertible_v<T,TT>> {};

template<typename T, typename... Targs>
constexpr static const bool is_convertible_all_v = is_convertible_all<T,Targs...>::value;

template <typename T>
struct is_sequence : std::false_type
{};

template <template <typename,auto...> typename Tcont, typename T, auto... tVs>
struct is_sequence<Tcont<T,tVs...>> : std::true_type
{};

template <typename T>
constexpr static const bool is_sequence_v = is_sequence<T>::value;



namespace constraint
{

template<bool v>
struct when
{
	template<typename Tret>
	using returns = typename std::enable_if_t<v,Tret>;

	template<typename Tret>
	using matches = swallow_t<Tret,returns<Tret>>;

	template<typename... Ts>
	using swallow = swallow_t<when<v>,Ts...>;

	constexpr static const bool value = v;
};


template<typename... Ts>
struct all : when<(Ts::value && ...)> {};

template<typename... Ts>
struct any : when<(Ts::value || ...)> {};


template<typename T1, typename T2>
struct base_of : when<std::is_base_of_v<T1,T2>> {};

template<typename T1, typename T2>
struct not_base_of : when<!std::is_base_of_v<T1,T2>> {};


template<typename T, typename... Ts>
struct base_of_all : when<is_base_of_all_v<T,Ts...>> {};

template<typename T, typename... Ts>
struct not_base_of_all : when<!is_base_of_all_v<T,Ts...>> {};


template<typename T>
struct container : when<is_container_v<T>> {};

template<typename T>
struct not_container : when<!is_container_v<T>> {};


template<typename T, template<typename...> typename Ttmpl>
struct instance_of : when<is_instance_of_v<T,Ttmpl>> {};

template<typename T, template<typename...> typename Ttmpl>
struct not_instance_of : when<!is_instance_of_v<T,Ttmpl>> {};


template<typename T>
struct integral : when<std::is_integral_v<T>> {};

template<typename T>
struct not_integral : when<!std::is_integral_v<T>> {};


template<typename T>
struct iterable : when<is_iterable_v<T>> {};

template<typename T>
struct not_iterable : when<!is_iterable_v<T>> {};


template<typename T>
struct back_emplacable : when<is_back_emplacable_v<T>> {};

template<typename T>
struct not_back_emplacable : when<!is_back_emplacable_v<T>> {};


template<typename T>
struct front_emplacable : when<is_front_emplacable_v<T>> {};

template<typename T>
struct not_front_emplacable : when<!is_front_emplacable_v<T>> {};


template<typename T1, typename... Tn>
struct one_of : when<is_one_of_v<T1,Tn...>> {};

template<typename T1, typename... Tn>
struct not_one_of : when<!is_one_of_v<T1,Tn...>> {};


template<typename T1, typename T2>
struct same : when<std::is_same_v<T1,T2>> {};

template<typename T1, typename T2>
struct not_same : when<!std::is_same_v<T1,T2>> {};


template<template<typename...> typename T1, template<typename...> typename T2>
struct same_template : when<is_same_template_v<T1,T2>> {};

template<template<typename...> typename T1, template<typename...> typename T2>
struct not_same_template : when<!is_same_template_v<T1,T2>> {};


template<typename Fn, typename... ArgTypes>
struct invocable : when<std::is_invocable_v<Fn,ArgTypes...>> {};

template<typename Fn, typename... ArgTypes>
struct not_invocable : when<!std::is_invocable_v<Fn,ArgTypes...>> {};

template<typename R, typename Fn, typename... ArgTypes>
struct invocable_r : when<std::is_invocable_r_v<R,Fn,ArgTypes...>> {};

template<typename R, typename Fn, typename... ArgTypes>
struct not_invocable_r : when<!std::is_invocable_r_v<R,Fn,ArgTypes...>> {};

template<typename T>
using valid = typename when<true>::swallow<T>;

namespace r
{

template<typename Tr, typename... Ts>
using all = typename constraint::all<Ts...>::template returns<Tr>;

template<typename Tr, typename... Ts>
using any = typename constraint::any<Ts...>::template returns<Tr>;


template<typename Tr, typename T1, typename T2>
using base_of = typename constraint::base_of<T1,T2>::template returns<Tr>;

template<typename Tr, typename T1, typename T2>
using not_base_of = typename constraint::not_base_of<T1,T2>::template returns<Tr>;


template<typename Tr, typename T, typename... Ts>
using base_of_all = typename constraint::base_of_all<T,Ts...>::template returns<Tr>;

template<typename Tr, typename T, typename... Ts>
using not_base_of_all = typename constraint::not_base_of_all<T,Ts...>::template returns<Tr>;


template<typename Tr, typename T>
using container = typename constraint::container<T>::template returns<Tr>;

template<typename Tr, typename T>
using not_container = typename constraint::not_container<T>::template returns<Tr>;


template<typename Tr, typename T, template<typename...> typename Ttmpl>
using instance_of = typename constraint::instance_of<T,Ttmpl>::template returns<Tr>;

template<typename Tr, typename T, template<typename...> typename Ttmpl>
using not_instance_of = typename constraint::not_instance_of<T,Ttmpl>::template returns<Tr>;


template<typename Tr, typename T>
using integral = typename constraint::integral<T>::template returns<Tr>;

template<typename Tr, typename T>
using not_integral = typename constraint::not_integral<T>::template returns<Tr>;


template<typename Tr, typename T>
using iterable = typename constraint::iterable<T>::template returns<Tr>;

template<typename Tr, typename T>
using not_iterable = typename constraint::not_iterable<T>::template returns<Tr>;

template<typename Tr, typename T>
using back_emplacable = typename constraint::back_emplacable<T>::template returns<Tr>;

template<typename Tr, typename T>
using not_back_emplacable = typename constraint::not_back_emplacable<T>::template returns<Tr>;


template<typename Tr, typename T>
using front_emplacable = typename constraint::front_emplacable<T>::template returns<Tr>;

template<typename Tr, typename T>
using not_front_emplacable = typename constraint::not_front_emplacable<T>::template returns<Tr>;

template<typename Tr, typename T1, typename T2, typename... Tn>
using one_of = typename constraint::one_of<T1,T2,Tn...>::template returns<Tr>;

template<typename Tr, typename T1, typename T2, typename... Tn>
using not_one_of = typename constraint::not_one_of<T1,T2,Tn...>::template returns<Tr>;


template<typename Tr, typename T1, typename T2>
using same = typename constraint::same<T1,T2>::template returns<Tr>;

template<typename Tr, typename T1, typename T2>
using not_same = typename constraint::not_same<T1,T2>::template returns<Tr>;


template<typename Tr, template<typename...> typename T1, template<typename...> typename T2>
using same_template = typename constraint::same_template<T1,T2>::template returns<Tr>;

template<typename Tr, template<typename...> typename T1, template<typename...> typename T2>
using not_same_template = typename constraint::not_same_template<T1,T2>::template returns<Tr>;

template<typename Tr, typename Fn, typename... ArgTypes>
using invocable = typename constraint::invocable<Fn,ArgTypes...>::template returns<Tr>;

template<typename Tr, typename R, typename Fn, typename... ArgTypes>
using invocable_r = typename constraint::invocable<R,Fn,ArgTypes...>::template returns<Tr>;
}

}

namespace cr = constraint::r;
}
#endif
