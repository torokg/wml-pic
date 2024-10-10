#ifndef EARPC_TYPES_CALL_HANDLE_H
# define EARPC_TYPES_CALL_HANDLE_H
# include <earpc/types/call_record.h>
namespace earpc {
namespace types {

template<typename TEnv>
struct call_handle
{
	typedef call_record<TEnv>              record_type;
	typedef typename TEnv::call_id_type    call_id_type;
	typedef typename TEnv::command_id_type command_id_type;
	typedef typename TEnv::address_type    address_type;

	template<typename T>
	using allocator = typename TEnv::template allocator<T>;

	const call_id_type         call_id;

	const command_id_type      command_id;

	const address_type         ip;

	const address_type         orig_ip;

	const uint8_t              reason;

	const std::vector<uint8_t,allocator<uint8_t>> arg_bytes;

	call_handle(const record_type &record, uint8_t preason)
		: call_id(record.call_id)
		, command_id(record.command_id)
		, ip(record.ip)
		, orig_ip(record.orig_ip)
		, arg_bytes(record.arg)
		, reason(preason)
	{}
};

template<typename TEnv>
struct incoming_call_handle : public call_handle<TEnv>
{
	typedef incoming_call_record<TEnv> record_type;

	typedef typename TEnv::time_point  time_point;

	time_point                         received;

	incoming_call_handle(const record_type &record, uint8_t preason)
		: call_handle<TEnv>(record,preason)
		, received(TEnv::clock::now())
	{}
};

template<typename TEnv>
struct outgoing_call_handle : public call_handle<TEnv>
{
	template<typename T>
	using allocator = typename TEnv::template allocator<T>;

	typedef outgoing_call_record<TEnv> record_type;

	typedef typename TEnv::time_point  time_point;

	const std::vector<uint8_t,allocator<uint8_t>> ret_bytes;
                                          
	time_point                         started;

	time_point                         activated;
                                          
	time_point                         finished;

	outgoing_call_handle(
		const record_type &record,
		const void *ret,
		uint16_t ret_size,
		uint8_t preason
	)
		: call_handle<TEnv>(record,preason)
		, started(record.started)
		, activated(record.activated)
		, finished(TEnv::clock::now())
		, ret_bytes(
			reinterpret_cast<const uint8_t*>(ret),
			ret?reinterpret_cast<const uint8_t*>(ret)+ret_size:0
		)
	{}
};

}}
#endif
