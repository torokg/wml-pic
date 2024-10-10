#ifndef TYPES_META_DESCRIBE_HH
# define TYPES_META_DESCRIBE_HH
# include <cstdint>
# include <utility>
namespace types::meta
{

// TODO: we need a static integer representation of arbitrary length for use here
//       henceforth, among numerous other reasons, types::integer shall also be incorporated to meta

template<typename T,typename=void>
struct value_count;

template<typename T>
constexpr uintmax_t value_count_v = value_count<T>::value;

template<>
struct value_count<void,void>
{ constexpr static const uintmax_t value = 0; };

template<>
struct value_count<bool,void>
{ constexpr static const uintmax_t value = 2; };

template<typename T1, typename T2>
struct value_count<std::pair<T1,T2>,void>
{ constexpr static const uintmax_t value = value_count_v<T1>*value_count_v<T2>; };

template<typename T, typename... Ts>
struct value_count<std::tuple<T,Ts...>,void>
{ constexpr static const uintmax_t value = value_count_v<T>*value_count_v<std::tuple<Ts...>>; };


template<typename T>
struct describe
{
	typedef std::decay_t<T>   value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	constexpr static const uintmax_t value_count = value_count_v<T>;

	// TODO: add all the constraints (partially) specialized to T
};
}

#endif
