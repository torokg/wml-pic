#ifndef TYPES_META_TYPE_LIST_HH
# define TYPES_META_TYPE_LIST_HH
# include <types/integer.hh>
namespace types::meta
{

template<template<typename...> typename T, typename... TBindArgs>
class bind;


namespace _detail::_type_list
{
	template<typename container1, typename container2>
	struct merge;

	template<bool all, typename container1, typename container2>
	struct subtract_list;

	template<typename container, template <typename> typename Tpred>
	struct has_characteristic;

	template<size_t index, typename... Tall>
	struct insert_at;

	template<
		typename container, 
		template<typename> typename Tpred, 
		off_t which, 
		typename... TnewEntries
	>
	struct insert_after;	

	template<
		typename container, 
		template<typename> typename Tpred, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	struct insert_after_or_at;

	template<
		typename container, 
		template<typename> typename Tpred, 
		off_t which, 
		typename... TnewEntries
	>
	struct insert_before;

	template<
		typename container, 
		template<typename> typename Tpred, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	struct insert_before_or_at;

	template<size_t from, size_t to, typename... Tall>
	struct remove_range;

	template<size_t from, size_t length, typename... Tall>
	struct sublist;

	template<typename list, size_t tBegin, size_t tEnd>
	struct splice;

	template<typename,size_t,size_t>
	struct remove_range_impl;

	template<typename container, typename item>
	struct count;

	template<bool all, typename container, typename... items>
	struct contains;

	template<bool all, typename container, typename items>
	struct contains_of;

	template<typename container, size_t index, typename Tx = void>
	struct tail_impl;

	template<typename container, size_t end>
	struct head_impl;

	template<typename container>
	struct reverse_impl;

	template<typename container, typename Tnew>
	struct intersperse_impl;

	template<typename container, template<typename> typename Tcond, template<typename> typename Ttrans>
	struct foreach_if_impl;

	template <typename,typename,typename=void>
	struct intersect;

	template<size_t I, typename tList, typename Visitor, typename... Targs>
	constexpr inline static auto visit_impl(std::enable_if_t<!tList::size,const size_t> key, Targs&&... args)
	{ return Visitor::visit(std::forward<Targs>(args)...); }

	template<size_t I, typename tList, typename Visitor, typename... Targs>
	constexpr inline static auto visit_impl(std::enable_if_t<(bool)tList::size,const size_t> key, Targs&&... args)
	{
		if(!key)
			return Visitor::template visit<
				I,
				typename tList::template get<0>
			>(std::forward<Targs>(args)...);

		return visit_impl<
				I+1,
				typename tList::template remove_front<1>,
				Visitor
			>(key-1,std::forward<Targs>(args)...);	
	}

}

/*! A size_t-indexable list of types */
template<typename... Tentries>
struct type_list;

template<typename T, typename... Tentries>
class type_list<T,Tentries...>
{

	typedef type_list<T,Tentries...> self;

public:

	class dynamic_entry 
	{
		typedef typename ::types::integer::ufit_t<sizeof...(Tentries)> index_type;

		const index_type index;

	public:
		typedef type_list<T,Tentries...> container;

		dynamic_entry() = delete;

		constexpr inline dynamic_entry(const size_t i)
			: index(static_cast<index_type>(i))
		{}
		
		constexpr inline operator size_t() const
		{ return index; }

		template<typename Visitor, typename... Targs>
		constexpr inline auto visit(Targs&&... args) const
		{ return container::template visit<Visitor>(*this, std::forward<Targs>(args)...); }

		template<typename Te>
		constexpr inline static dynamic_entry make()
		{ return dynamic_entry(container::template first_index_of<Te>); }
	};

	/*! Evaluates to the number of entries in current type list */
	constexpr static const size_t size = sizeof...(Tentries)+1;

	/*! Evaluates to the tIndex'th element of current type list */
	template<size_t tIndex>
	using get = std::conditional_t<
		!tIndex,
		T,
		typename type_list<Tentries...>::template get<tIndex-1>
	>;

	template<typename Te>
	constexpr inline static dynamic_entry get_entry()
	{ return dynamic_entry::template make<Te>(); }

	template<typename Visitor, typename... Targs>
	static auto visit(dynamic_entry key, Targs&&... args)
	{ return _detail::_type_list::visit_impl<0,self,Visitor,Targs...>(key,std::forward<Targs>(args)...); }

	/*! Evaluates to the number of occurrences of Te within current type list */
	template<typename Te>
	constexpr static const size_t count = _detail::_type_list::count<self, Te>::value;

	/*! Evaluates to true when Tpred is satisfied by any element from the current type list*/
	template<template <typename> typename Tpred>
	constexpr static const bool has_characteristic = 
		_detail::_type_list::has_characteristic<self,Tpred>::value;

	/*! Evaluates to true when Te is contained by current type list */
	template<typename Te>
	constexpr static const bool contains = is_one_of_v<Te,T,Tentries...>;

	/*! Evaluates to true when any base of Te is contained by current type list */
	template<typename Te>
	constexpr static const bool contains_base_of = is_base_of_any_v<Te,T,Tentries...>;

	/*! Evaluates to true when any child of Te is contained by current type list */
	template<typename Te>
	constexpr static const bool contains_child_of = is_child_of_any_v<Te,T,Tentries...>;

	/*! Evaluates to true when any relative (i.e base or child) of Te is contained by current type list */
	template<typename Te>
	constexpr static const bool contains_relative_of = is_relative_of_any_v<Te,T,Tentries...>;

	template<typename Te>
	constexpr static const size_t first_index_of
		= first_where<bind<std::is_same,Te>::template type,T,Tentries...>::index;

	template<typename Te>
	using first_base_of = typename first_where<bind<is_child_of,Te>::template type,T,Tentries...>::type;

    /*! Evaluates to a sublist of current type list where every item is a base class of Te.
	    Order remains the same as it has been in the original list */
	//template<typename Te>
	//using bases_of = ...

	template<typename Te>
	using first_child_of = typename first_where<meta::bind<std::is_base_of,Te>::template type,T,Tentries...>::type;

    /*! Evaluates to a sublist of current type list where every item is a subclass of Te.
	    Order remains the same as it has been in the original list */
	//template<typename Te>
	//using children_of = ...

	template<typename Te>
	using first_relative_of = typename first_where<meta::bind<is_relative_of,Te>::template type,T,Tentries...>::type;

    /*! Evaluates to a sublist of current type list where every item is a relative (either base or child) class of Te.
	    Order remains the same as it has been in the original list */
	template<typename Te>
	using relatives_of = typename std::conditional_t<
		is_relative_of_v<Te,T>,
		type_list<Te>,
		type_list<>
	>::template append_list<
		typename type_list<Tentries...>
		::template relatives_of<Te>
	>;

	template<template<typename> typename Tf>
	using filter = typename std::conditional_t<
		Tf<T>::value,
		type_list<T>,
		type_list<>
	>::template append_list<
		typename type_list<Tentries...>
		::template filter<Tf>
	>;

	template<typename Te>
	using first_convertible = typename first_where<bind<std::is_convertible,Te>::template type,T,Tentries...>::type;


	/*! Evaluates to true when current type list contains any of the specified template parameters. */
	template<typename... Ts>
	constexpr static const bool contains_any = _detail::_type_list::contains<false,self,Ts...>::value;

	/*! Evaluates to true when current type list contains any elements of the specified meta container. */
	template<typename Tcont>
	constexpr static const bool contains_any_of = _detail::_type_list::contains_of<false,self,Tcont>::value;


	/*! Evaluates to true when current type list contains all of the specified template parameters. */
	template<typename... Ts>
	constexpr static const bool contains_all = _detail::_type_list::contains<true,self,Ts...>::value;

	/*! Evaluates to true when current type list contains every element of the specified meta container. */
	template<typename Tcont>
	constexpr static const bool contains_all_of = _detail::_type_list::contains_of<true,self,Tcont>::value;

	/*! Evaluates to a type list that contains the given list of entries before
	    those contained by current type list */
	template<typename... TnewEntries>
	using prepend = type_list<TnewEntries..., T, Tentries...>;

	/*! Evaluates to a type list that contains the given list of entries after
	    those contained by current type list */
	template<typename... TnewEntries>
	using append = type_list<T, Tentries...,TnewEntries...>;

	/*! Evaluates to the concatenation of current type list and the given type list
	    (respectively)*/
	template<typename T2>
	using append_list = typename _detail::_type_list::merge<self,T2>::type;

	/*! Evaluates to the concatenation of the given type list and the current type list
	    (respectively) */
	template<typename T2>
	using prepend_list = typename _detail::_type_list::merge<T2,self>::type;

	/*! Gets a sublist of the current type list corresponding to the given index range. */
	template<size_t tBegin, size_t tLength>
	using sublist = typename _detail::_type_list::sublist<tBegin, tLength, self>::type;

	/*!  */
	//template<integer::sequ_t<size_t> tBegin,integer::sequ_t<size_t> tEnd = size>
	//using splice = 
	//	typename _detail::_type_list::splice<self, balance_idx_v<tBegin,size>, balance_idx_v<tEnd,size>>::type;

	/*! Evaluates to a type list constructed by extending current one with the given entries at the given position */
	template<integer::sequ_t<size_t> index, typename... TnewEntries>
	using insert_at = 
		typename _detail::_type_list::insert_at<
			index < 0 ? index + size + 1 : index, 
			self, 
			TnewEntries...
		>::type;

	/*! */
	template<template<typename,typename> typename Tpred, typename TT, off_t which, typename... TnewEntries>
	using insert_after = 
		typename _detail::_type_list::insert_after<
			self,bind<Tpred,TT>::template type,which,TnewEntries...
		>::type;

	/*! */
	template<
		template<typename,typename> typename Tpred, 
		typename TT, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	using insert_after_or_at = 
		typename _detail::_type_list::insert_after_or_at<
			self,
			bind<Tpred,TT>::template type,
			which,
			at,
			TnewEntries...
		>::type;

	/*! Inserts all of TnewEntries after the first occurrence of TT in the orig list */
	template<typename TT, typename... TnewEntries>
	using insert_after_first_match = 
		typename self::template insert_after<std::is_same,TT,0,TnewEntries...>;

	/*! */
	template<template<typename,typename> typename Tpred, typename TT, off_t which, typename... TnewEntries>
	using insert_before = 
		typename _detail::_type_list::insert_before<
			self,bind<Tpred,TT>::template type,which,TnewEntries...
		>::type;

	/*! */
	template<
		template<typename,typename> typename Tpred, 
		typename TT, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	using insert_before_or_at = 
		typename _detail::_type_list::insert_before_or_at<
			self,
			bind<Tpred,TT>::template type,
			which,
			at,
			TnewEntries...
		>::type;

	/*! Inserts all of TnewEntries before the first occurrence of TT in the orig list */
	template<typename TT, typename... TnewEntries>
	using insert_before_first_match = 
		typename self::template insert_before<std::is_same,TT,0,TnewEntries...>;

	/*! Evaluates to a type list which contains all the elements of current type list
	    but the first occurrences of the specified types */
	template<typename... Tes>
	using remove = typename _detail::_type_list::subtract_list<false,self, type_list<Tes...>>::type;

	/*! Evaluates to a type list which contains all the elements of current type list
	    but the first occurrences of the items in the specified type list */
	template<typename T2>
	using remove_list = typename _detail::_type_list::subtract_list<false,self,T2>::type;

	/*! Evaluates to a type list which contains all the elements of current type list
	    but all occurrences of the specified types */
	template<typename... Tes>
	using remove_all = typename _detail::_type_list::subtract_list<true,self, type_list<Tes...>>::type;

	/*! Evaluates to a type list which contains all the elements of current type list
	    but all occurrences of the items in the specified type list */
	template<typename T2>
	using remove_list_all = typename _detail::_type_list::subtract_list<true,self,T2>::type;

	template<size_t count>
	using remove_front = typename _detail::_type_list::tail_impl<self, count>::type;

	/*! Removes elements in the specified range */
	// using remove_range = typename _detail::_type_list::remove_range<tBegin,tEnd,self>::type;
	//template<integer::sequ_t<size_t> tBegin,integer::sequ_t<size_t> tEnd = size>
	//using remove_range = typename _detail::_type_list::remove_range_impl<self, balance_idx_v<tBegin,size>, balance_idx_v<tEnd,size>>::type;

	/*! Removes element at the specified index */
	//template<size_t tPos>
	//using remove_at = type_list<T,Tentries...>::template remove_range<tPos, tPos+1>;

	/*! Evaluates to a type list which is a suffix of current list with the length of size-index. */
	template<size_t index>
	using tail = typename _detail::_type_list::tail_impl<self, size - index>::type;

	/*! Evaluates to a type list which is a prefix of current list with the length of end. */
	template<size_t end>
	using head = typename _detail::_type_list::head_impl<self, end>::type;

	/*! Gives the reversed list */
	using reverse = typename _detail::_type_list::reverse_impl<self>::type;

	/*! Takes an element and a list and `intersperses' that element between the elements of the list.
		For example:
			typle_list<T1, T2, T3, T4, T5>::intersperse<A> -> type_list<T1, A, T2, A, T3, A, T4, A, T5> */
	template<typename Tnew>
	using intersperse = typename _detail::_type_list::intersperse_impl<self,Tnew>::type;

////// TODO proper place of these
	template<template <typename> typename Tcond>
	struct Condition
	{ 
		template <typename Tparam>
		constexpr static const bool value = Tcond<Tparam>::value; 
	};

	template<template <typename> typename Ttrans>
	struct Transform
	{ 
		template <typename Tparam>
		using type = typename Ttrans<Tparam>::type; 
	};

//////

	/*! Applies a transformation to each of the list elements, if condition matches. */
	template<template<typename> typename Tcond, template<typename> typename Ttrans>
	using foreach_if = typename _detail::_type_list::template foreach_if_impl<self,Tcond,Ttrans>::type;

	/*! Same as fold_if, but Transform is applied on each element */
	template<template<typename> typename Ttrans>
	using foreach = typename _detail::_type_list::template foreach_if_impl<self,true_swallow,Ttrans>::type;

	template<typename Tthat>
	using intersect = typename _detail::_type_list::template intersect<self,Tthat>::type;

	/*! Concatenates two lists. */
	// concat = ...

	/*! Concatenates two lists applying a Transform on each element. Additionaly Condition for applyong the
		transformation can be added.
		For example:
			list::concatMap<Transform, Condition = std::true_type> */
	// concatMap = ...

	/*! Returns the conjunction of a bool list */
	// and = ...

	/*! Returns the disjunction of a bool list */
	// or = ...

	/*! Calculates the + operator equivalent result of static type members, if this has meaning. */
	// sum = ...

	/*! Calculates the * operator equivalent result of static type members, if this has meaning. */
	// product = ...

	/*! Calculates the maximum result of static type members, if this has meaning and the type of members
		has operator<. */
	// maximum = ...

	/*! Calculates the minimum result of static type members, if this has meaning and the type of members
		has operator<. */
	// minimum = ...

	/*! Same as list_foreach_if, but in the result type_list will be present type lists, in that form,
		that each element conatins the original and the transformed type value.
		This should be useful, when testing list_foreach_if result. */
	// scanl_if = ...

	/*! In analogous way as scanl_if */
	// scanl = ...

	/*! Applies a Transform object to each element of a type_list, and parallelly gives back the result
		of an operation given simultaneously in a type_list.
		For example:
			type_list<T1, T2, T3>::mapAccum<Ttransform, Tmath> ->
			type_list< Taccumulated, type_list<T1', T2', T3'> >
		 */
	// mapAccum = ...

	/*! Creates a type_list with given Tparam elements, Tlength times in a new list.
		For example:
			type_list<T>::replicate<5> ->
			type_list<T, T, T, T, T> */
	// replicate = ...

	/*! Creates a new type_list with Tn times the content of the parameter type_list.
		For example:
			type_list<T1, T2, T3>::cycle<3> ->
			type_list<T1, T2, T3, T1, T2, T3, T1, T2, T3> */
	// cycle = ...

	/*! The unfoldr function is a `dual' to foldr: while foldr reduces a list to a summary value,
		unfoldr builds a list from a seed value. The function takes the element and returns
		Nothing if it is done producing the list or returns Just (a,b), in which case,
		a is a prepended to the list and b is used as the next element in a recursive call.  */
	// unfoldr = ...

	/*!  */
	// take = ...

	/*!  */
	// drop = ...

	/*!  */
	// dropLastN = ...

	/*!  */
	// dropLast = ...

	/*!  */
	// splitAt = ...

	/*!  */
	// takeWhile = ...

	/*!  */
	// dropWhile = ...

	/*!  */
	// span = ...

	/*!  */
	// break = ...

	/*!  */
	// group = ...

	/*!  */
	// inits = ...

	/*!  */
	// tails = ...

	/*!  */
	// isPrefixOf = ...

	/*!  */
	// isSuffixOf = ...

	/*!  */
	// elem = ...

	/*!  */
	// notElem = ...

	/*!  */
	// lookup = ...

	/*!  */
	// find = ...

	/*!  */
	// filter = ...

	/*! Calls Fun(Elem) on successive elements Elem of List1. Fun/1 must return either a Boolean
		or a tuple {true, Value}. The function returns the list of elements for which Fun
		returns a new value, where a value of true is synonymous with {true, Elem}. */
	// filterMap = ...

	/*! Unfolds the given list, so that it will not contain any sublists */
	// flatten

	/*! Unfolds the given list, and counts its length. It's like the sum of sublists and element in the list */
	// flatlength =

	/*! Applies transform on the flattened list.
		Takes a function from As to lists of Bs, and a list of As (List1) and produces a list of Bs by
		applying the function to every element in List1 and appending the resulting lists.
		That is, flatmap behaves as if it had been defined as follows:

			flatmap(Fun, List1) ->
				append(map(Fun, List1)).
	 */
	// flat_foreach =

	// keydelete

	// keyMap

	// keyFind

	// keymember

	// keymerge

	// keyreplace

	// keysearch

	// keysort

	// keystore

	// keytake

	/*! Splits the list by a Condition */
	// partition

	/*! Returns true if the list contains the parameter list */
	// contains

	/*! Returns a type_list with wrapped basic constrictible and enumerable elements within the given range */
	// seq

	/*! Returns a type_list which is split up at the given position */
	// split

	/*! Returns a new type_list substracted from it the elements of the given parameter list */
	// substract
	
	/*! Retruns if the lists are each one's prefix */
	// prefix

	/*! Retruns if the lists are each one's prefix */
	// suffix  
	
	/*!  */
	// partition = ...

	/*!  */
	// elemIndex = ...

	/*!  */
	// elemIndices = ...

	/*!  */
	// findIndex = ...

	/*!  */
	// findIndices = ...

	/*!  */
	// zipWith = ...

	// Reviewed until here

	/*!  */
	// unzip = ...
	
	/*!  */
	// lines = ...

	/*!  */
	// words = ...

	/*!  */
	// unlines = ...

	/*!  */
	// unwords = ...

	/*!  */
	// nub = ...

	/*!  */
	// erase = ...

	/*!  */
	// (\\) = ...

	/*!  */
	// union = ...

	/*!  */
	// intersect = ...

	/*!  */
	// sort = ...

	/*!  */
	// insert = ...

	/*!  */
	// nubBy = ...

	/*!  */
	// deleteBy = ...

	/*!  */
	// deleteFirstsBy = ...

	/*!  */
	// unionBy = ...

	/*!  */
	// intersectBy = ...

	/*!  */
	// groupBy = ...

	/*!  */
	// sortBy = ...

	/*!  */
	// insertBy = ...

	/*!  */
	// maximumBy = ...

	/*!  */
	// minimumBy = ...

	/*!  */
	// genericLength = ...

	/*!  */
	// genericTake = ...

	/*!  */
	// genericDrop = ...

	/*!  */
	// genericSplitAt = ...

	/*!  */
	// genericIndex = ...

	/*!  */
	// genericReplicate = ...

	// Further work can be:
	// related on sources:
	// 	- https://www.haskell.org/hugs/pages/libraries/base/Data-List.html
	// 	- http://erlang.org/doc/man/lists.html
	 	
	/*! Evaluates to a specialization of given template where elements of current type
	    list are applied as template parameters, with respect to the order */
	template<template<typename...> typename Ttarget>
	using apply = Ttarget<T,Tentries...>;

	/*! Evaluates to a type which contains all but the first element of current type list */
	typedef type_list<Tentries...> remove_first;

	/*! Removes the last element of the list */
	//typedef remove_at<sizeof...(Tentries)> remove_last;

	/*! Evaluates to the first element of current type list */
	typedef get<0> first;

	/*! Evaluates to the last element of current type list */
	typedef get<sizeof...(Tentries)> last;
};

template<>
struct type_list<>
{
	constexpr static const size_t size = 0;

	template<typename Te>
	constexpr static const bool contains = false;

	template<typename Visitor>
	static auto visit(size_t key)
	{ return Visitor::visit(); }

	template<typename... TnewEntries>
	using prepend = type_list<TnewEntries...>;

	template<typename... TnewEntries>
	using append = type_list<TnewEntries...>;

	template<typename T2>
	using append_list = typename _detail::_type_list::merge<type_list<>,T2>::type;

	template<typename Te>
	using relatives_of = type_list<>;

	template<template<typename> typename Tf>
	using filter = type_list<>;

	typedef type_list<> reverse;

	template<typename... Tes>
	using remove_all = type_list<>;

	template<typename...>
	constexpr static const bool contains_relative_of = false;

	/*! */
	template<template<typename,typename> typename Tpred, typename TT, off_t which, typename... TnewEntries>
	using insert_after = append<TnewEntries...>;

	/*! */
	template<
		template<typename,typename> typename Tpred, 
		typename TT, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	using insert_after_or_at = append<TnewEntries...>;

	/*! Inserts all of TnewEntries after the first occurrence of TT in the orig list */
	template<typename TT, typename... TnewEntries>
	using insert_after_first_match = append<TnewEntries...>;

	/*! */
	template<template<typename,typename> typename Tpred, typename TT, off_t which, typename... TnewEntries>
	using insert_before = prepend<TnewEntries...>;

	/*! */
	template<
		template<typename,typename> typename Tpred, 
		typename TT, 
		off_t which, 
		off_t at, 
		typename... TnewEntries
	>
	using insert_before_or_at = prepend<TnewEntries...>;

	/*! Inserts all of TnewEntries before the first occurrence of TT in the orig list */
	template<typename TT, typename... TnewEntries>
	using insert_before_first_match = prepend<TnewEntries...>;

	template<typename... Tes>
	using remove = type_list<>;

	template<size_t tIndex>
	using get = void;

	template<size_t tBegin, size_t tEnd>
	using sublist = type_list<>;

	template<template<typename...> typename Ttarget>
	using apply = Ttarget<>;
};

template<typename... Ts>
using tlist = type_list<Ts...>;

namespace _detail::_type_list
{

template<typename T>
struct count<type_list<>,T>
{ constexpr static const size_t value = 0; };

template<typename T, typename Te, typename... Tes>
struct count<type_list<Te,Tes...>,T>
{
	constexpr static const size_t value = (std::is_same_v<T,Te> ? 1 : 0) + count<type_list<Tes...>,T>::value;
};


template<typename... Ts1, typename... Ts2>
struct merge<type_list<Ts1...>, type_list<Ts2...>>
{ typedef type_list<Ts1...,Ts2...> type; };


template<bool all, typename container, typename item>
struct subtract;

template<bool tAll, typename T2>
struct subtract<tAll,type_list<>,T2>
{ typedef type_list<> type; };

template<bool tAll, typename T1, typename... Ts1, typename T2>
struct subtract<tAll, type_list<T1,Ts1...>, T2>
{
	typedef typename std::conditional_t<
		std::is_same_v<T1,T2>,
		std::conditional_t<
			tAll,
			typename subtract<tAll,type_list<Ts1...>,T2>::type,
			type_list<Ts1...>
		>,
		typename type_list<T1>
			::template append_list<
				typename subtract<tAll,type_list<Ts1...>,T2>::type
			>

	> type;
};


template<bool tAll, typename... Ts>
struct subtract_list<tAll, type_list<>, type_list<Ts...>>
{ typedef type_list<> type; };

template<bool tAll, typename... Ts>
struct subtract_list<tAll, type_list<Ts...>, type_list<>>
{ typedef type_list<Ts...> type; };

template<bool tAll>
struct subtract_list<tAll, type_list<>, type_list<>>
{ typedef type_list<> type; };

template<bool tAll, typename... Ts1, typename T2, typename... Ts2>
struct subtract_list<tAll,type_list<Ts1...>, type_list<T2,Ts2...>>
{
	typedef typename subtract_list<
		tAll,
		typename subtract<tAll,type_list<Ts1...>,T2>::type,
		type_list<Ts2...>
	>::type type;
};

template <size_t tBegin, size_t tLength, typename Thead, typename... Ttail>
struct sublist<tBegin, tLength, type_list<Thead, Ttail...>>
{
	typedef std::conditional_t<
		tBegin,
		typename type_list<Ttail...>::template sublist<tBegin-1, tLength>,
		std::conditional_t<
			tLength,
			typename type_list<Thead>::template append_list<
				typename type_list<Ttail...>::template sublist<tBegin, tLength-1>
			>,
			type_list<>
		>
	> type;
};

template<template<typename> typename Tpred, typename T,typename... Ts>
struct has_characteristic<type_list<T,Ts...>, Tpred> 
	: std::bool_constant<
		Tpred<T>::value || 
		has_characteristic<type_list<Ts...>,Tpred>::value
	>
{};

template<template<typename> typename Tpred>
struct has_characteristic<type_list<>, Tpred> 
	: std::false_type
{};

template <size_t index, typename... Telem, typename... TnewEntries>
class insert_at<index, type_list<Telem...>, TnewEntries...>
{
	static_assert( !(index > type_list<Telem...>::size), "Index must be less than list length!");

	typedef typename type_list<Telem...>::template sublist<0, index> begin;
	typedef typename type_list<Telem...>::template sublist<index, type_list<Telem...>::size> second;
	typedef typename begin::template append<TnewEntries...> first;

public:
	typedef typename first::template append_list<second> type;
};

template<template<typename> typename Tpred, off_t which, off_t at, typename... Ts, typename... TnewEntries>
struct insert_after_or_at<type_list<Ts...>, Tpred, which, at, TnewEntries...>
{
	// TODO relation between off_t and sequ<size_t>
	constexpr static const size_t list_size = type_list<Ts...>::size;

	typedef std::conditional_t<
		type_list<Ts...>::template has_characteristic<Tpred>,
		typename insert_after<
			type_list<Ts...>,
			Tpred,
			which, 
			TnewEntries...
		>::type,
		typename type_list<Ts...>::template insert_at<at,TnewEntries...>
	> type;
};

template<template<typename> typename Tpred, off_t which, typename T, typename... Ts, typename... TnewEntries>
struct insert_after<type_list<T,Ts...>, Tpred, which, TnewEntries...>
{
	typedef std::conditional_t<
		Tpred<T>::value,
		std::conditional_t<
			which,
			typename type_list<T>::template append_list<
				typename insert_after<
					type_list<Ts...>, Tpred, which-1, TnewEntries...
				>::type
			>,
			type_list<T, TnewEntries..., Ts...>
		>,
		typename type_list<T>::template append_list<
			typename insert_after<
				type_list<Ts...>, Tpred, which, TnewEntries...
			>::type
		>
	> type;
};

template<template<typename> typename Tpred, off_t which, typename... TnewEntries>
struct insert_after<type_list<>, Tpred, which, TnewEntries...>
{
	// TODO decay

	typedef type_list<> type;
	// typedef std::conditional_t<
	// 	(Tpred<T>::value && which == 0),
	// 	type_list<T, TnewEntries...>,
	// 	type_list<>
	// > type;
};

template<template<typename> typename Tpred, off_t which, off_t at, typename... Ts, typename... TnewEntries>
struct insert_before_or_at<type_list<Ts...>, Tpred, which, at, TnewEntries...>
{
	// TODO relation between off_t and sequ<size_t>
	constexpr static const size_t list_size = type_list<Ts...>::size;

	typedef std::conditional_t<
		type_list<Ts...>::template has_characteristic<Tpred>,
		typename insert_before<
			type_list<Ts...>,
			Tpred,
			which, 
			TnewEntries...
		>::type,
		typename type_list<Ts...>::template insert_at<at,TnewEntries...>
	> type;
};

template<template<typename> typename Tpred, off_t which, typename T, typename... Ts, typename... TnewEntries>
struct insert_before<type_list<T,Ts...>, Tpred, which, TnewEntries...>
{
	typedef std::conditional_t<
		Tpred<T>::value,
		std::conditional_t<
			which,
			typename type_list<T>::template append_list<
				typename insert_before<
					type_list<Ts...>, Tpred, which-1, TnewEntries...
				>::type
			>,
			type_list<TnewEntries..., T, Ts...>
		>,
		typename type_list<T>::template append_list<
			typename insert_before<
				type_list<Ts...>, Tpred, which, TnewEntries...
			>::type
		>
	> type;
};

template<template<typename> typename Tpred, off_t which, typename... TnewEntries>
struct insert_before<type_list<>, Tpred, which, TnewEntries...>
{
	// TODO decay

	typedef type_list<> type;
};

template<typename container, typename item, typename... items>
struct contains<true,container,item,items...>
	: std::bool_constant<
		container::template contains<item> &&
		contains<true,container,items...>::value
> {};

template<typename container, typename item, typename... items>
struct contains<false,container,item,items...>
	: std::bool_constant<
		container::template contains<item> ||
		contains<false,container,items...>::value
> {};

template<bool all,typename container>
struct contains<all,container> : std::bool_constant<all> {};

template<bool all, typename container, template<typename> typename Tct, typename... Ts>
struct contains_of<all,container,Tct<Ts...>> : contains<all,container,Ts...> {};

template <size_t tBegin, size_t tEnd, typename... Telem>
class remove_range<tBegin, tEnd, type_list<Telem...>>
{
	typedef typename type_list<Telem...>::template sublist<0, tBegin> begin;
	typedef typename type_list<Telem...>::template
		sublist<tEnd+1, type_list<Telem...>::size> end;

public:
	typedef typename begin::template append_list<end> type;
};

template<size_t index, typename T, typename... Ts>
struct tail_impl<type_list<T,Ts...>,index,std::enable_if_t<index>>
{
	typedef typename tail_impl<type_list<Ts...>, index - 1>::type  type;
};
template<size_t index, typename T, typename... Ts>
struct tail_impl<type_list<T,Ts...>,index,std::enable_if_t<!index>>
{
	typedef type_list<T,Ts...> type;
};

template<size_t index>
struct tail_impl<type_list<>,index,void>
{
	typedef type_list<> type;
};

template<size_t end, typename T, typename... Ts>
struct head_impl<type_list<T,Ts...>,end>
{
	typedef std::conditional_t<
		end,
		typename type_list<T>::template append_list<
			typename head_impl<type_list<Ts...>, end - 1>::type
		>,
		type_list<>
	> type;
};

template<size_t end>
struct head_impl<type_list<>,end>
{
	typedef type_list<> type;
};

template<typename T, typename... Ts>
struct reverse_impl<type_list<T,Ts...>>
{
	typedef typename type_list<T>::template prepend_list<
		typename reverse_impl<type_list<Ts...>>::type
	> type;
};

template<typename T>
struct reverse_impl<type_list<T>>
{
	typedef type_list<T> type;
};

template<typename Tnew,typename T, typename... Ts>
struct intersperse_impl<type_list<T,Ts...>,Tnew>
{
	typedef typename type_list<T,Tnew>::template append_list<
		typename intersperse_impl<type_list<Ts...>,Tnew>::type
	> type;
};

template<typename Tnew, typename T>
struct intersperse_impl<type_list<T>,Tnew>
{
	typedef type_list<T,Tnew> type;
};

template<template<typename> typename Tcond, template<typename> typename Ttrans, typename... Ts>
struct foreach_if_impl<type_list<Ts...>, Tcond, Ttrans>
{
	typedef type_list<
		typename std::conditional_t<
			Tcond<Ts>::value,
			Ttrans<Ts>,
			Ts
		>...
	> type;
};

}

namespace _detail::_type_list
{
/*	template<size_t tBegin, size_t tEnd, typename... Tr>
	struct splice<
		type_list<Tr...>,
		tBegin, tEnd
	>
	{
		typedef std::conditional_t<
			(tBegin <= tEnd),
			typename splice_impl<type_list<Tr...>,tBegin,tEnd>::type,
			typename remove_range_impl<typename type_list<Tr...>,tEnd,tBegin>::type
		> type; 
	};

	template<typename, size_t, size_t, typename...>
	struct splice_impl;

	template<size_t tBegin, size_t tEnd, typename T, typename... Tr>
	struct splice_impl<
		type_list<T,Tr...>,
		tBegin, tEnd,
		std::enable_if_t<tBegin>
	>
	{
		typedef typename splice_impl<type_list<Tr...>, tBegin-1, tEnd-1>::type type;
	};

	template<size_t tBegin, size_t tEnd, typename T, typename... Tr>
	struct splice_impl<
		type_list<T,Tr...>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && tEnd>
	>
	{
		typedef typename type_list<T>::template append_list<
			typename splice_impl<type_list<Tr...>, tBegin, tEnd-1>::type
		> type;
	};

	template<size_t tBegin, size_t tEnd>
	struct splice_impl<
		type_list<>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && tEnd>
	>
	{
		typedef type_list<> type;
	};

	template<size_t tBegin, size_t tEnd, typename... Tr>
	struct splice_impl<
		type_list<Tr...>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && !tEnd>
	>
	{
		typedef type_list<> type;
	};

	template<size_t tBegin, size_t tEnd, typename T, typename... Tr>
	struct remove_range_impl<
		type_list<T,Tr...>,
		tBegin, tEnd,
		std::enable_if_t<tBegin>
	>
	{
		typedef typename type_list<T>::template append_list<
			typename remove_range_impl<type_list<Tr...>, tBegin-1, tEnd-1>::type
		> type;
	};
	
	template<size_t tBegin, size_t tEnd, typename T, typename... Tr>
	struct remove_range_impl<
		type_list<T,Tr...>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && tEnd>
	>
	{
		typedef typename type_list<>::template append_list<
			typename remove_range_impl<type_list<Tr...>, tBegin, tEnd-1>::type
		> type;
	};

	template<size_t tBegin, size_t tEnd>
	struct remove_range_impl<
		type_list<>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && tEnd>
	>
	{
		typedef type_list<> type;
	};

	template<size_t tBegin, size_t tEnd, typename T, typename... Tr>
	struct remove_range_impl<
		type_list<T,Tr...>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && !tEnd>
	>
	{
		typedef type_list<Tr...> type;
	};

	template<size_t tBegin, size_t tEnd>
	struct remove_range_impl<
		type_list<>,
		tBegin, tEnd,
		std::enable_if_t<!tBegin && !tEnd>
	>
	{
		typedef type_list<> type;
	};
	*/
}

}
#endif
