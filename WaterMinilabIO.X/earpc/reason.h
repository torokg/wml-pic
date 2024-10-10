#ifndef EARPC_REASON_H
# define EARPC_REASON_H
namespace earpc {

struct reason
{
	static const uint8_t success = 0;
	static const uint8_t process = 0;
	static const uint8_t expired = 1;
	static const uint8_t rejected = 2;
	static const uint8_t cancelled = 3;
	static const uint8_t size_mismatch = 4;
};

}
#endif
