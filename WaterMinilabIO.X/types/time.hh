#ifndef TYPES_TIME_HH
# define TYPES_TIME_HH
# include <type_traits>
# include <chrono>
# include <sstream>
# include <iomanip>
namespace types::time
{

	// duration to nanoseconds
	constexpr int64_t nsec(std::chrono::nanoseconds t)
	{ return t.count(); }

	template<typename Trep, typename Tratio>
	constexpr int64_t nsec(std::chrono::duration<Trep,Tratio> t)
	{ return nsec(std::chrono::duration_cast<std::chrono::nanoseconds>(t)); }

	// time_point to nanoseconds
	template<typename Tclock>
	constexpr uint64_t nsec(std::chrono::time_point<Tclock,std::chrono::nanoseconds> t)
	{ return nsec(t.time_since_epoch()); }

	template<typename Tclock, typename Trep>
	constexpr uint64_t nsec(std::chrono::time_point<Tclock,Trep> t)
	{ return nsec(std::chrono::time_point_cast<std::chrono::nanoseconds>(t)); }

	// duration to microseconds
	constexpr int64_t usec(std::chrono::microseconds t)
	{ return t.count(); }

	template<typename Trep, typename Tratio>
	constexpr int64_t usec(std::chrono::duration<Trep,Tratio> t)
	{ return usec(std::chrono::duration_cast<std::chrono::microseconds>(t)); }

	template<typename Trep, typename Tratio>
	constexpr double fusec(std::chrono::duration<Trep,Tratio> t)
	{ return static_cast<double>(nsec(t))/1000; }

	// time_point to microseconds
	template<typename Tclock>
	constexpr uint64_t usec(std::chrono::time_point<Tclock,std::chrono::microseconds> t)
	{ return usec(t.time_since_epoch()); }

	template<typename Tclock, typename Trep>
	constexpr uint64_t usec(std::chrono::time_point<Tclock,Trep> t)
	{ return usec(std::chrono::time_point_cast<std::chrono::microseconds>(t)); }

	// duration to milliseconds
	constexpr int64_t msec(std::chrono::milliseconds t)
	{ return t.count(); }

	template<typename Trep, typename Tratio>
	constexpr int64_t msec(std::chrono::duration<Trep,Tratio> t)
	{ return msec(std::chrono::duration_cast<std::chrono::milliseconds>(t)); }

	template<typename Trep, typename Tratio>
	double fmsec(std::chrono::duration<Trep,Tratio> t)
	{ return static_cast<double>(nsec(t))/1000000; }

	template<typename Tclock, typename Trep>
	double fmsec(std::chrono::time_point<Tclock,Trep> t)
	{ return static_cast<double>(nsec(t))/1000000; }

	// time_point to milliseconds
	template<typename Tclock>
	constexpr uint64_t msec(std::chrono::time_point<Tclock,std::chrono::milliseconds> t)
	{ return msec(t.time_since_epoch()); }

	template<typename Tclock, typename Trep>
	constexpr uint64_t msec(std::chrono::time_point<Tclock,Trep> t)
	{ return msec(std::chrono::time_point_cast<std::chrono::milliseconds>(t)); }

	// duration to seconds
	constexpr int64_t sec(std::chrono::seconds t)
	{ return t.count(); }

	template<typename Trep, typename Tratio>
	constexpr int64_t sec(std::chrono::duration<Trep,Tratio> t)
	{ return sec(std::chrono::duration_cast<std::chrono::seconds>(t)); }

	template<typename Trep, typename Tratio>
	constexpr double fsec(std::chrono::duration<Trep,Tratio> t)
	{ return static_cast<double>(nsec(t))/1000000000l; }

	template<typename Tclock, typename Trep>
	constexpr double fsec(std::chrono::time_point<Tclock,Trep> t)
	{ return static_cast<double>(nsec(t))/1000000000l; }

	// time_point to seconds
	template<typename Tclock>
	constexpr uint64_t sec(std::chrono::time_point<Tclock,std::chrono::seconds> t)
	{ return sec(t.time_since_epoch()); }

	template<typename Tclock, typename Trep>
	constexpr uint64_t sec(std::chrono::time_point<Tclock,Trep> t)
	{ return sec(std::chrono::time_point_cast<std::chrono::seconds>(t)); }

	namespace format
	{
		class format_data 
		{
			constexpr static const uint64_t unix_epoc_U = 62168515200000000ULL;

			format_data(const uint64_t pU, bool pabs);
		public:
			bool absolute;  // true for time_point, false for duration
			uint64_t U;	// microseconds since abs 0
			uint64_t s;	// seconds since abs 0
			uint64_t h;     // hours since abs 0
			uint32_t d;	// days since abs 0
			uint32_t q;	// leap years since abs 0
			uint32_t F;	// fraction seconds
			uint16_t qd;	// day of leap-year cycle
			uint16_t Y;	// year
			uint16_t m;	// month
			uint16_t D;     // day of month
			uint16_t yd;	// day of year
			uint16_t H;	// hours
			uint16_t M;	// minutes
			uint16_t S;	// seconds

			template<typename Tclock, typename Trep = typename Tclock::duration>
			inline format_data(std::chrono::time_point<Tclock,Trep> t)
				: format_data(usec(t)+unix_epoc_U, true)
			{}

			template<typename Trep, typename Tratio>
			format_data(std::chrono::duration<Trep,Tratio> dur)
				: format_data(usec(dur), false)
			{}

		};

		template<typename Tformat>
		struct basic_format : protected format_data
		{
			template<typename T>
			inline static std::string to_string(T&& r)
			{ return Tformat(std::forward<T>(r)); }

		protected:
			template<typename T>
			inline basic_format(T&& r)
				: format_data(std::forward<T>(r))
			{}
		};

		class usual : public basic_format<usual>
		{
			friend class basic_format<usual>;

			template<typename T>
			inline usual(T&& r)
				: basic_format<usual>(std::forward<T>(r))
			{}

			operator std::string() const;
		};

		class iso8601 : public basic_format<iso8601>
		{
			friend class basic_format<iso8601>;
			
			template<typename T>
			inline iso8601(T&& r)
				: basic_format<iso8601>(std::forward<T>(r))
			{}

			operator std::string() const;
		};

		class YMD : public basic_format<YMD>
		{
			friend class basic_format<YMD>;
			
			template<typename T>
			inline YMD(T&& r)
				: basic_format<YMD>(std::forward<T>(r))
			{}

			operator std::string() const;
		
		};

		class YMDHMS : public basic_format<YMDHMS>
		{
			friend class basic_format<YMDHMS>;
			
			template<typename T>
			inline YMDHMS(T&& r)
				: basic_format<YMDHMS>(std::forward<T>(r))
			{}

			operator std::string() const;
		
		};

		class yMD : public basic_format<yMD>
		{
			friend class basic_format<yMD>;
			
			template<typename T>
			inline yMD(T&& r)
				: basic_format<yMD>(std::forward<T>(r))
			{}

			operator std::string() const;
		
		};
	}

	template<typename Tformat = format::usual, typename T = const std::chrono::high_resolution_clock::time_point&>
	inline std::string timestamp(T&& t)
	{
		static_assert(
			std::is_base_of_v<format::basic_format<Tformat>, Tformat>,
			"a format class T must be a subclass of format::basic_format<T>"
		);
		return Tformat::to_string(std::forward<T>(t));
	}
}

template<typename Tclock, typename Trep>
inline std::ostream&
operator<<(std::ostream &o, std::chrono::time_point<Tclock, Trep> &&t)
{ return o << types::time::timestamp(std::forward<std::chrono::time_point<Tclock, Trep>>(t)); }

template<typename Trep, typename Tratio>
inline std::ostream&
operator<<(std::ostream &o, std::chrono::duration<Trep, Tratio> &&t)
{ return o << types::time::timestamp(std::forward<std::chrono::duration<Trep, Tratio>>(t)); }

#endif
