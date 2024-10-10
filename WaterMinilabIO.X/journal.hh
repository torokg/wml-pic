#ifndef IOCLOUD_JOURNAL_JOURNAL_DUMMY_HH
# define IOCLOUD_JOURNAL_JOURNAL_DUMMY_HH
# include <cstdint>
# include <string>
# include <ostream>

class journal
{
    typedef
      std::basic_ostream
       < char
       , std::char_traits<char>
       >
      ostream_type;
    
    typedef
      std::basic_string
       < char
       , std::char_traits<char>
       , std::allocator<char>
       >
      string_type;

	typedef ostream_type& (*iomanip_fn_t)(ostream_type&);

public:
	struct end_type {};

	constexpr static const int16_t min_level = -1;

	constexpr static const int16_t default_level = -1;

	constexpr static const end_type end = end_type();

	constexpr static const uint8_t fatal = 0;
	constexpr static const uint8_t critical = 16;
	constexpr static const uint8_t error = 32;
	constexpr static const uint8_t warning = 48;
	constexpr static const uint8_t info = 64;
	constexpr static const uint8_t debug = 80;
	constexpr static const uint8_t trace = 96;

	constexpr static void init()
	{}

	constexpr static void uninit()
	{}

	constexpr static uint8_t domain_level(const string_type &domain)
	{ return 0; }

	constexpr static void domain_level(const string_type &domain,uint8_t level)
	{}

	constexpr static bool is_busy()
	{ return false; }

	constexpr journal(uint8_t plevel, const string_type &pdomain)
	{}

	constexpr journal &operator<<(iomanip_fn_t)
	{ return *this; }

	template<typename T>
	constexpr journal &operator<<(T&&)
	{ return *this; }

};
# if defined(JOURNAL_TRANSPORT)
#  include <io/interop/transport/string.hh>
#  include <io/interop/transport/container.hh>
#  ifdef __linux__
#   define __declspec(X)
#  endif
namespace transport = io::interop::transport;

__declspec(dllexport)
extern "C"
void journal_write(uint8_t level, transport::string &domain, transport::string &content)
{}

__declspec(dllexport)
extern "C"
void journal_minimum_level_get(uint8_t &level) 
{}

__declspec(dllexport)
extern "C"
void journal_minimum_level_set(uint8_t level)
{}

__declspec(dllexport)
extern "C"
void journal_default_level_get(uint8_t &level)
{}

__declspec(dllexport)
extern "C"
void journal_default_level_set(uint8_t level)
{}

__declspec(dllexport)
extern "C"
void journal_domains_get(transport::container<std::string> &content)
{}

__declspec(dllexport)
extern "C"
void journal_level_get(transport::string & domain, uint8_t &level)
{}

__declspec(dllexport)
extern "C"
void journal_level_set(transport::string &domain, uint8_t level)
{}
# endif
#endif
