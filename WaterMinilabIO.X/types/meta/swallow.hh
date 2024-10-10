#ifndef TYPES_META_SWALLOW_HH
# define TYPES_META_SWALLOW_HH
# include <cstddef>
namespace types::meta
{

/*! Defines a member type which is equivalent to the first template parameter. */
template<typename T, typename...>
struct swallow
{ typedef T type; };

template<typename T, typename... Ts>
using swallow_t = typename swallow<T,Ts...>::type;


/*! Defines a consexpr member which is of type T and is equal to v. */
template<typename T, T v, typename... Ts>
struct constant_swallow
{ constexpr static const T value = v; };

template<typename T, T v, typename... Ts>
constexpr T constant_swallow_v = constant_swallow<T,v,Ts...>::value;

template<bool v, typename... Ts>
struct bool_swallow : constant_swallow<bool,v,Ts...> {};

template<bool v, typename... Ts>
constexpr bool bool_swallow_v = bool_swallow<v,Ts...>::value;

template<typename... Ts>
struct true_swallow : bool_swallow<true,Ts...> {};

template<typename... Ts>
constexpr bool true_swallow_v = true_swallow<Ts...>::value;

template<typename... Ts>
struct false_swallow : bool_swallow<false,Ts...> {};

template<typename... Ts>
constexpr bool false_swallow_v = true_swallow<Ts...>::value;

template<size_t v, typename... Ts>
struct size_swallow : constant_swallow<size_t, v, Ts...> {};

template<size_t v, typename... Ts>
constexpr bool size_swallow_v = size_swallow<v,Ts...>::value;

}
#endif
