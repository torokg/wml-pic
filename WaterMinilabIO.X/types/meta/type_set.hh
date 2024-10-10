#ifndef TYPES_META_TYPE_SET_HH
# define TYPES_META_TYPE_SET_HH
namespace types::meta
{

namespace _detail::_type_set
{
	template<typename, typename Tcond, typename...>
	struct insert_impl;

	template<typename...>
	struct insert_list_impl;
}

template<typename... Ts>
class type_set
{
	typedef type_set<Ts...>self;
public:

	typedef type_list<Ts...> as_type_list;

	constexpr static const size_t size = sizeof...(Ts);

	static_assert(are_distinct_v<Ts...>, "Duplicate item in type set");

	/*! Evaluates to current type set extended with the given entries */
	template <typename... TnewEntries>
	using insert = typename as_type_list::template append_list< 
		typename _detail::_type_set::insert_impl<as_type_list, void, TnewEntries...>::type
	>::template apply<type_set>;

	/*! Evaluates to current type set extended with the elements in the given type list */
	template<typename T2>
	using insert_list = typename _detail::_type_set::insert_list_impl<self,T2>::type;

	/*! Evaluates to a type set which contains all the elements of current type set
	    but the specified types */
	template<typename... Tes>
	using remove = typename as_type_list::template remove<Tes...>::template apply<type_set>;

	/*! Evaluates to a type set which contains all the elements of current type set
	    but the items in the specified type list */
	template<typename T2>
	using remove_list = typename as_type_list::template remove_list<T2>::template apply<type_set>;

	//TODO:
	//	union, intersection, difference, symmetric difference, ...
};

template<>
struct type_set<>
{
	typedef type_list<> as_type_list;

	constexpr static const size_t size = 0;

	// TODO compiles only with -fpermissive
	// template <typename... TnewEntries>
	// using insert = typename as_type_list::template append_list< 
	// 	typename _detail::_type_set::insert_impl<as_type_list, void, TnewEntries...>::type
	// >::template apply<type_set>;

	template <typename T2>
	using insert_list = typename _detail::_type_set::insert_list_impl<type_set<>, T2>::type;

	template<typename... Tes>
	using remove = type_set<>;

	/*! Evaluates to a type set which contains all the elements of current type set
	    but the items in the specified type list */
	template<typename T2>
	using remove_list = type_set<>;
};

namespace _detail::_type_set
{
	template <typename Tcont, typename T, typename... Tr>
	struct insert_impl<
		Tcont,
		std::enable_if_t<Tcont::template contains<T>>,
		T,
		Tr...
	>
	{
		typedef typename type_list<>::template append_list<
			typename insert_impl<Tcont,void,Tr...>::type
		> type;
	};

	template <typename Tcont, typename T, typename... Tr>
	struct insert_impl<
		Tcont,
		std::enable_if_t<!Tcont::template contains<T>>,
		T,
		Tr...
	>
	{
		typedef typename type_list<T>::template append_list<
			typename insert_impl<typename Tcont::template append<T>,void,Tr...>::type
		> type;
	};


	template<typename Tcont, typename T>
	struct insert_impl<
		Tcont,
		std::enable_if_t<Tcont::template contains<T>>,
		T
	>
	{
		typedef type_list<> type;
	};

	template<typename Tcont, typename T>
	struct insert_impl<
		Tcont,
		std::enable_if_t<!Tcont::template contains<T>>,
		T
	>
	{
		typedef type_list<T> type;
	};

	template<typename Tset, typename... Targs>
	struct insert_list_impl<Tset,type_list<Targs...>>
	{
		typedef typename Tset::template insert<Targs...> type;
	};
}

}
#endif
