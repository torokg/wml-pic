#ifndef EARPC_TYPES_HEADER_H
# define EARPC_TYPES_HEADER_H
# include <net/algorithm.h>

namespace earpc {
namespace types {

#pragma pack(push,1)
template<typename TEnv>
struct header 
{
	typename TEnv::command_id_type command_id;

	typename TEnv::call_id_type    call_id;

	uint16_t        checksum;

	bool checksum_create()
	{
		*const_cast<volatile uint16_t*>(&checksum) = 0;
		checksum = ::net::algorithm::hton(net::algorithm::checksum_finish(
			net::algorithm::checksum_add(this,sizeof(header<TEnv>))
		));
		return true;
	}

	bool checksum_verify() const
	{ return net::algorithm::checksum_verify(this,sizeof(header<TEnv>)); }
};
#pragma pack(pop)

}}
#endif
