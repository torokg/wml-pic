#ifndef TYPES_INTEGER_H
# define TYPES_INTEGER_H
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# include <sys/types.h>
# include <type_traits>
# include <cstdint>
# ifdef min
#  undef min
# endif
# ifdef max
#  undef max
# endif
// typedef __int128 int128_t;
// typedef unsigned __int128 uint128_t;

namespace types::integer
{

//! Provides a type equivalent to the argument that has the greatest range
template<typename... Ts>
struct widest;

template<typename T>
struct widest<T>
{ typedef T type; };

template<typename Ta, typename Tb, typename... Ts>
struct widest<Ta,Tb,Ts...> : std::conditional<
	(sizeof(Ta) < sizeof(typename widest<Tb,Ts...>::type)),
	typename widest<Tb,Ts...>::type,
	Ta
> {};

template<typename... Ts>
using widest_t = typename widest<Ts...>::type;


//! Provides a type equivalent to the argument that has the smallest range
template<typename... Ts>
struct tightest;

template<typename T>
struct tightest<T>
{ typedef T type; };

template<typename Ta, typename Tb, typename... Ts>
struct tightest<Ta,Tb,Ts...> : std::conditional<
	(sizeof(Ta) < sizeof(typename tightest<Tb,Ts...>::type)),
	Ta,
	typename tightest<Tb,Ts...>::type
> {};

template<typename... Ts>
using tightest_t = typename tightest<Ts...>::type;


//! Provides a type large enough for \a value
/*! Provides an unsigned integral member type <code>type</code>, which is large enough for
    <code>value</code> to fit in
*/
template<uint64_t value>
struct ufit
{
	//! An unsigned integral type in which \a value fits
	typedef typename std::conditional<
		value <= static_cast<uint8_t>(-1),
		uint8_t,
		typename std::conditional<
			value <= static_cast<uint16_t>(-1),
			uint16_t,
			typename std::conditional<
				value <= static_cast<uint32_t>(-1),
				uint32_t,
				uint64_t
			>::type
		>::type
	>::type type;
};

template<uint64_t value>
using ufit_t = typename ufit<value>::type;

//! Provides a type which consists of at least \a bits bits
/*! Provides an unsigned integral member type <code>type</code>, which consists of at least
	<code>bits</code> bits
  */
template<uint8_t bits>
struct ubits
{
	static_assert(bits <= 64, "The largest unsigned integer representation is 64 bits long");

	typedef typename std::conditional<
		bits <= 8,
		uint8_t,
		typename std::conditional <
			bits <= 16,
			uint16_t,
			typename std::conditional <
				bits <= 32,
				uint32_t,
				uint64_t
			>::type
		>::type
	>::type type;
};

template<uint8_t value>
using ubits_t = typename ubits<value>::type;

//! Provides a type which consists of at least \a bits bits
/*! Provides a signed integral member type <code>type</code>, which consists of at least
	<code>bits</code> bits
  */
template<uint8_t bits>
struct sbits
{
	static_assert(bits <= 64, "The largest signed integer representation is 64 bits long");

	typedef typename std::conditional<
		bits <= 8,
		int8_t,
		typename std::conditional <
			bits <= 16,
			int16_t,
			typename std::conditional <
				bits <= 32,
				int32_t,
				int64_t
			>::type
		>::type
	>::type type;
};

template<uint8_t value>
using sbits_t = typename sbits<value>::type;

//! Provides a type which consists of at least \a bits bits
/*! Provides a signed integral member type <code>type</code>, which is the signed equivalent
	of <code>T</code>
 */
template<typename T>
struct sequ
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);
	typedef typename sbits<sizeof(T)*8>::type type;
};

template<typename T>
using sequ_t = typename sequ<T>::type;

//! Provides a type which consists of at least \a bits bits
/*! Provides an unsigned integral member type <code>type</code>, which is the unsigned equivalent
	of <code>T</code>
 */
template<typename T>
struct uequ
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);
	typedef typename ubits<sizeof(T)*8>::type type;
};

template<typename T>
using uequ_t = typename uequ<T>::type;

//! Provides a type large enough for \a value
/*! Provides an unsigned integral member type <code>type</code>, which is large enough for
	<code>value</code> to fit in
  */
template<int64_t value>
struct sfit
{ typedef typename sequ<typename ufit<static_cast<uint64_t>(value)>::type>::type type; };

template<uint64_t value>
using sfit_t = typename sfit<value>::type;

//! Provides a type which consists of exactly half as much bits as  \a T
/*! Provides an integral member type <code>type</code>, which is half as long as <code>T</code>.
	<code>type</code> holds the signedness of <code>T</code>
 */
template<typename T>
struct half
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);

	typedef typename std::conditional_t<
		std::is_signed<T>::value,
		sbits_t<sizeof(T)*4>,
		ubits_t<sizeof(T)*4>
	> type;
};

template <typename T>
using half_t = typename half<T>::type;

template <typename T>
struct dbl
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);

	typedef typename std::conditional_t<
		std::is_signed<T>::value,
		sbits_t<sizeof(T)*16>,
		ubits_t<sizeof(T)*16>
	> type;
};

template <typename T>
using dbl_t = typename dbl<T>::type;

template<typename T>
constexpr T abs(T value)
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);

	return (value < 0) ? -value : value;
}

template<typename T>
constexpr
std::enable_if_t<std::is_same_v<T,uequ_t<T>>,T> log2(T t)
{
	if(t <= 1)
		return 0;
	else
		return log2(t>>1)+1;
}

template<unsigned tBase,typename T>
void itoa(T value, char *buffer, unsigned size)
{
	static_assert(tBase <= 16,"base must be at most 16");
	static_assert(tBase > 1,"base must be at least 2");
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);

	constexpr static const char *digits = "0123456789abcdef";

	if(!size)
		return;

	--size;
	if(!size)
	{
		buffer[0] = 0;
		return;
	}

	int i = 0;
	if(value)
	{
		if(value < 0)
		{
			*buffer++ = '-';
			--size;
			value *= -1;
		}
		for(; value > 0 && i < size; value /= tBase)
			buffer[i++] = digits[value%tBase];
	}
	else
		buffer[i++] = '0';
	buffer[i] = 0;
	if(i > 1)
	{
		const int l = i;
		for(--i; i >= l/2; --i)
		{
			auto &x = buffer[i];
			auto &y = buffer[l-i-1];
			const auto t = x;
			x = y;
			y = t;
		}
	}
}

template<unsigned tBase=10, typename T>
void atoi(T &value, const char *buffer, size_t size)
{
	static_assert(tBase <= 16,"base must be at most 16");
	static_assert(tBase > 1,"base must be at least 2");
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);

	if(!size--)
		return;

	int i = 0;

	for(value = 0; i < size; ++i)
	{
		const uint8_t d = static_cast<uint8_t>(buffer[i]);
		if(d != ' ' && d != '\t' && d != '\v' && d != '\r' && d != '\n')
			break;
	}
	const char s = buffer[i];
	if(s == '-' || s == '+')
		++i;

	for(value = 0; i < size; ++i)
	{
		value *= tBase;
		const uint8_t d = static_cast<uint8_t>(buffer[i]);
		if(d >= '0' && d <= '9')
			value += d-'0';
		else if(d >= 'A' && d <= 'F')
			value += d-'A';
		else if(d >= 'a' && d <= 'f')
			value += d-'a';
		else break;
	}
	if(s == '-')
		value *= -1;
}

template<unsigned tBase=10, typename T, typename StringType>
inline void atoi(T &value, const StringType&& s)
{ atoi(value,s.c_str(),s.size()); }



struct byteorder
{
#ifdef __BYTE_ORDER__
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	static const bool big_endian = false;
	static const bool little_endian = true;
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	static const bool big_endian = true;
	static const bool little_endian = false;
# endif
#else
#  error Unable to determine byteorder
#endif

};

template<typename T, T v>
struct static_swap_byteorder
{
	static_assert(
		std::is_integral<T>::value,
		"value must be of an integral type"
	);
private:
	typedef half_t<T> S;
	static const uint8_t shift_bits = sizeof(S)*8;
	static const S upper = static_cast<S>(v >> shift_bits);
	static const S lower = static_cast<S>(v & ~(uequ_t<T>(-1) << shift_bits));
public:

	static const T value =
		(static_cast<T>(static_swap_byteorder<S,lower>::value) << (sizeof(S))*8) |
		static_swap_byteorder<S,upper>::value;
};

template<uint8_t v>
struct static_swap_byteorder<uint8_t, v>
{ static const uint8_t value = v; };

template<typename T, T v>
constexpr T static_swap_byteorder_v = static_swap_byteorder<T,v>::value;

template<typename T>
T swap_byteorder(T value)
{
	static_assert(
		std::is_integral<T>::value ||
		std::is_enum<T>::value,
		"value must be of an integral type"
	);

	T r;

	const uint8_t *a = reinterpret_cast<uint8_t*>(&value)+sizeof(T);
	uint8_t *b = reinterpret_cast<uint8_t*>(&r)-1;

	for(int i = 0; i < sizeof(T); ++i)
		*++b = *--a;

	return r;
}

template<typename T>
constexpr T min(T t)
{ return t; }

template<typename Ta, typename Tb, typename... Ts>
constexpr tightest_t<Ta,Tb,Ts...> min(Ta a, Tb b, Ts... s)
{
	const auto r = min(b,s...);
	return (a<r)?a:r;
}

template<typename T>
constexpr T max(T t)
{ return t; }

template<typename Ta, typename Tb, typename... Ts>
constexpr widest_t<Ta,Tb,Ts...> max(Ta a, Tb b, Ts... s)
{
	const auto r = max(b,s...);
	return (a<r)?r:a;
}

template<uint8_t v>
using uint8_constant = std::integral_constant<uint8_t,v>;

template<uint16_t v>
using uint16_constant = std::integral_constant<uint16_t,v>;

template<uint32_t v>
using uint32_constant = std::integral_constant<uint32_t,v>;

template<uint64_t v>
using uint64_constant = std::integral_constant<uint64_t,v>;

template<uintmax_t v>
using uintmax_constant = std::integral_constant<uintmax_t,v>;

template<size_t v>
using size_constant = std::integral_constant<size_t,v>;

template<uintptr_t v>
using uintptr_constant = std::integral_constant<uintptr_t,v>;

template<int8_t v>
using int8_constant = std::integral_constant<int8_t,v>;

template<int16_t v>
using int16_constant = std::integral_constant<int16_t,v>;

template<int32_t v>
using int32_constant = std::integral_constant<int32_t,v>;

template<int64_t v>
using int64_constant = std::integral_constant<int64_t,v>;

template<intmax_t v>
using intmax_constant = std::integral_constant<intmax_t,v>;

template<intptr_t v>
using intptr_constant = std::integral_constant<intptr_t,v>;

}

#endif
