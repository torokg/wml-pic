#ifndef TYPES_META_HH
# define TYPES_META_HH
# include <functional>
# include <cstring>
# include <type_traits>
# include <tuple>
# include <types/meta/metaitem.hh>
# include <types/meta/annotation.hh>
# include <types/meta/describe.hh>
# include <types/meta/pair.hh>

# include <types/meta/template_wrapper.hh>
# include <types/meta/despecialize.hh>
# include <types/meta/swallow.hh>
# include <types/meta/constraint.hh>
# include <types/meta/ctmap.hh>

namespace types::meta
{
template <typename... Ts> struct overload : Ts... { using Ts::operator()...; };
template <typename... Ts> overload(Ts...) -> overload<Ts...>;


namespace _detail
{
	template <typename Tuple, typename F, size_t... Indices>
	void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
		using swallow = int[];
		(void)swallow{ 1,
			(f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
		};
	}


}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f)
{
	constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
	_detail::for_each_impl(
		std::forward<Tuple>(tuple), std::forward<F>(f), 
		std::make_index_sequence<N>{}
	);
}

namespace _detail
{
	template<size_t N, template<typename> typename Tpred, typename... Ts>
	struct first_where;

	template<size_t N, template<typename> typename Tpred, typename T, typename... Ts>
	struct first_where<N, Tpred,T,Ts...>
	{
		template<bool = Tpred<T>::value, typename = void>
		struct evaluate;

		template<typename Tx>
		struct evaluate<true,Tx>
		{
			typedef T type;
			constexpr static const size_t index = N;
		};

		template<typename Tx>
		struct evaluate<false,Tx>
		{
			typedef typename first_where<N+1, Tpred, Ts...>::template evaluate<>::type type;
			constexpr static const size_t index = first_where<N+1, Tpred, Ts...>::template evaluate<>::index;
		};
	};

	template<size_t N, template<typename> typename Tpred>
	struct first_where<N, Tpred> {};
}

// TODO: these should be removed as type list provide them.
template<template<typename> typename Tpred, typename... Ts>
struct first_where : _detail::first_where<0,Tpred,Ts...>::template evaluate<> {};

template<template<typename> typename Tpred, typename... Ts>
using first_where_t = typename first_where<Tpred,Ts...>::type;

template<template<typename> typename Tpred, typename... Ts>
constexpr size_t first_where_v = first_where<Tpred,Ts...>::index;

}

# include <types/meta/type_list.hh>
# include <types/meta/bind.hh>
# include <types/meta/type_set.hh>
# include <types/meta/template_arguments_of.hh>

namespace types::meta
{

template<typename... Tentries>
struct type_map;

template<typename Tkey, typename Tvalue, typename... Tkeys, typename... Tvalues>
struct type_map<std::pair<Tkey,Tvalue>, std::pair<Tkeys,Tvalues>...>
{
	constexpr static const size_t        size = sizeof...(Tkeys)+1;

	typedef type_list<Tkey,Tkeys...>     keys;

	typedef type_list<Tvalue,Tvalues...> values;

	template<typename T>
	using get = typename std::conditional_t<
		std::is_same_v<Tkey,T>,
		Tvalue,
		typename type_map<std::pair<Tkeys,Tvalues>...>::template get<T>
	>;
};

template<>
struct type_map<>
{
	typedef type_list<> keys;

	typedef type_list<> values;

	template<typename T>
	using get = void;
};

// TODO
/*
template<typename...>
struct make_type_map;

template<typename... Tkeys, typename... Tvalues>
struct make_type_map<type_list<Tkeys...>,type_list<Tvalues...>>
{
	static_assert(sizeof...(Tkeys) == sizeof...(Tvalues), "key and value lists must be of the same length");

	// typedef type_map<std::pair<Tkeys,Tvalues>...> type;
};

template<typename Tkeys, typename Tvalues>
using make_type_map_t = typename make_type_map<Tkeys,Tvalues>::type;
*/

namespace _detail
{
	template<typename... Ts>
	class map_payload;

	template <typename T, typename... Targs>
	struct map_payload<T,Targs...> : map_payload<Targs...>
	{
		typedef typename T::first_type  key_type;

		typedef typename T::second_type value_type;

		template<typename Tx, typename Tc = void>
		struct _get_type;

		template<typename Tx>
		struct _get_type<Tx,
			std::enable_if_t<std::is_same_v<Tx,key_type>>
		>
		{ typedef value_type type; };

		template<typename Tx>
		struct _get_type<Tx,
			std::enable_if_t<!std::is_same_v<Tx,key_type>>
		>
		{ typedef typename map_payload<Targs...>::template _get_type<Tx>::type type; };


		template<typename Tx>
		using get_type = typename _get_type<Tx,void>::type;

		value_type value;

		template <typename Tx>
		inline std::enable_if_t<
			std::is_same_v<Tx, typename T::first_type>, 
			value_type&
		> get()
		{ return value; }

		template <typename Tx>
		inline std::enable_if_t<
			!std::is_same_v<Tx, typename T::first_type>, 
			get_type<Tx>&
		> get()
		{ return map_payload<Targs...>::template get<Tx>(); }

		template <typename Tx>
		inline std::enable_if_t<
			std::is_same_v<Tx, typename T::first_type>, 
			const value_type&
		> get() const
		{ return value; }

		template <typename Tx>
		inline std::enable_if_t<
			!std::is_same_v<Tx, typename T::first_type>, 
			const get_type<Tx>&
		> get() const
		{ return map_payload<Targs...>::template get<Tx>(); }
	};

	template<>
	struct map_payload<>
	{
		template<typename Tx>
		struct _get_type
		{ static_assert(std::is_same_v<std::void_t<Tx>,void>,"No such element"); };

		template <typename Tx>
		void get() const
		{ static_assert(std::is_same_v<std::void_t<Tx>,void>,"No such element"); }

		template<typename Tx>
		struct get_type{};

	};

}

template <typename... Targs>
class static_keyed_map
{
	typedef typename type_list<Targs...>::reverse::template apply<_detail::map_payload> payload_t;

	payload_t payload;

public:

	typedef type_list<typename Targs::first_type...>  keys;

	typedef type_list<typename Targs::second_type...> value_types;

	constexpr static const size_t size = sizeof...(Targs);

	typedef typename value_types::reverse::template apply<std::tuple> tuple_type;

	template<typename Tx>
	using get_type = typename payload_t::template get_type<Tx>;

	template<typename Tx>
	inline auto &get()
	{ return payload.template get<Tx>(); }

	template<typename Tx>
	inline const auto &get() const
	{ return payload.template get<Tx>(); }

	inline operator tuple_type&()
	{ return reinterpret_cast<tuple_type&>(*this); }

	inline operator const tuple_type&() const
	{ return reinterpret_cast<const tuple_type&>(*this); }

	inline tuple_type &as_tuple()
	{ return static_cast<tuple_type&>(*this); }

	inline const tuple_type &as_tuple() const
	{ return static_cast<const tuple_type&>(*this); }

};

template<typename... Ts>
using tvtuple = static_keyed_map<Ts...>;

template<size_t Start, size_t Count,typename... Ts>
inline auto &subtuple(std::tuple<Ts...> &t)
{
	typedef typename type_list<Ts...>
		::template sublist<Start,Count>
	l;

	constexpr const size_t i = Start+Count-1;

	return reinterpret_cast<
		typename l
		::template apply<std::tuple>&
	>(static_cast<typename l::template get<i>&>(std::get<i>(t)));
}

template<size_t Start, size_t Count,typename... Ts>
inline const auto &subtuple(const std::tuple<Ts...> &t)
{
	typedef typename type_list<Ts...>
		::template sublist<Start,Count>
	l;

	constexpr const size_t i = Start+Count-1;

	return reinterpret_cast<
		const typename l
		::template apply<std::tuple>&
	>(static_cast<const typename l::template get<i>&>(std::get<i>(t)));
}

template<typename T, T... tEntries>
struct value_list;

template<typename T, T v, T v2, T... tEntries>
struct value_list<T,v,v2,tEntries...>
{
	typedef T value_type;
	constexpr static const size_t size = sizeof...(tEntries)+2;
	
	template<size_t tIndex>
	constexpr static T get = !tIndex ? v : value_list<T,v2,tEntries...>::template get<tIndex-1>;

	template<template<T...> typename Ttarget>
	using apply = Ttarget<v,v2,tEntries...>;
};

template<typename T, T v>
struct value_list<T,v>
{
	typedef T value_type;
	constexpr static const size_t size = 1;
	
	template<size_t tIndex>
	constexpr static std::enable_if_t<!tIndex,T> get = v;

	template<template<T...> typename Ttarget>
	using apply = Ttarget<v>;
};

template<typename T>
struct value_list<T>
{
	typedef T type;
	constexpr static const size_t size = 0;

	template<template<T...> typename Ttarget>
	using apply = Ttarget<>;
};

template<typename T, T... vs>
using clist = value_list<T>;



template<size_t... Tentries>
struct size_list : value_list<size_t, Tentries...>{};

template<uintptr_t... Tentries>
struct uintptr_list : value_list<uintptr_t, Tentries...>{};

template<size_t index>
struct argument
{
	template<typename T, typename... Targs>
	constexpr auto get_from(T a, Targs... args)
	{
		static_assert(index < sizeof...(Targs)+1, "argument index out of range");
		return argument<index-1>::get_from(args...);
	}
};

template<>
struct argument<0>
{
	template<typename T, typename... Targs>
	constexpr T get_from(T a, Targs... args)
	{ return a; }
};


template <typename Tcont, typename Tnew>
struct rebind_sequence_container;

template <typename Tprev, typename Talloc, template <typename, typename> typename Tcontainer, typename Tnew>
struct rebind_sequence_container<Tcontainer<Tprev, Talloc>, Tnew >
{
    typedef Tcontainer<Tnew, typename Talloc::template rebind<Tnew>::other> type;
};

template <typename Tcont, typename Tnew>
using rebind_sequence_container_t = typename rebind_sequence_container<Tcont, Tnew>::type;

}

namespace std
{

template<size_t I, typename... Ts>
struct tuple_element<I,::types::meta::static_keyed_map<Ts...>>
{ typedef typename ::types::meta::static_keyed_map<Ts...>::value_types::template get<I> type; };

template<size_t I, typename... Ts>
constexpr
typename tuple_element_t<I,::types::meta::static_keyed_map<Ts...>>::type&
get(::types::meta::static_keyed_map<Ts...> &m)
{ return m.template get<typename decltype(m)::keys::template get<I>>(); }
	
template<size_t I, typename... Ts>
constexpr
typename tuple_element_t<I,::types::meta::static_keyed_map<Ts...>>::type&
get(const ::types::meta::static_keyed_map<Ts...> &m)
{ return m.template get<typename decltype(m)::keys::template get<I>>(); }

}

#endif
