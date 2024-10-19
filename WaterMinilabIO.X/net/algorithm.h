#ifndef NET_ALGORITHM_H
# define NET_ALGORITHM_H
# include <algorithm>
# include <types/integer.hh>
namespace net::algorithm
{
	template<typename T>
	std::remove_reference_t<std::remove_cv_t<T>>
	hton(T value)
	{
		static_assert(
			std::is_integral<T>::value,
			"value must be of an integral type"
		);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        decltype(hton(value)) rv = value;

		volatile uint8_t * const x = reinterpret_cast<uint8_t*>(&rv);

		for(int i = 0; i < sizeof(rv)/2; ++i)
				std::swap(x[i],x[sizeof(T)-1-i]);

		return rv;
#else
        return value;
#endif
	}

	bool checksum_create(volatile void *target, uint16_t length, uint16_t checksum_pos);

	uint32_t checksum_add(const volatile void *source, uint16_t length);

	uint16_t checksum_finish(uint32_t s);

	inline bool checksum_verify(uint32_t s)
	{ return !checksum_finish(s); }

	inline bool checksum_verify(const volatile void *source, uint16_t length)
	{ return checksum_verify(checksum_add(source,length)); }
}
#endif
