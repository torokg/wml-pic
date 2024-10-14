#ifndef EARPC_BUFFER_CALL_RECORD_H
# define EARPC_BUFFER_CALL_RECORD_H
# include <vector>
# include <cstdint>
# include <chrono>
# include <algorithm>
# include <functional>

#ifdef min
# undef min
#endif
namespace earpc {
namespace types {

template<typename TEnv>
struct call_record
{

#pragma region Environment declarations
	typedef typename TEnv::command_id_type  command_id_type;

	typedef typename TEnv::call_id_type     call_id_type;

	typedef typename TEnv::address_type     address_type;

	template<typename T>
	using allocator = typename TEnv::template allocator<T>;

	typedef typename TEnv::time_point       time_point;

	typedef typename TEnv::clock            clock;

	static const uint32_t call_timeout = TEnv::call_timeout;
#pragma endregion


	address_type         ip;

	address_type         orig_ip;

	command_id_type      command_id;

	call_id_type         call_id;

	time_point           expiry;

	std::vector<uint8_t,allocator<uint8_t>> arg;

	uint16_t             return_size;

	call_record(
		address_type  pip,
		command_id_type    pcommand_id,
		call_id_type       pcall_id,
		const void        *argp,
		uint16_t           args,
		uint16_t           rets
	)
		: ip(pip)
		, orig_ip(pip)
		, command_id(pcommand_id)
		, call_id(pcall_id)
		, arg(
			reinterpret_cast<const uint8_t*>(argp),
			reinterpret_cast<const uint8_t*>(argp)+args
		)
		, return_size(rets)
		, expiry(
			clock::now() + std::chrono::milliseconds(
				static_cast<uint32_t>(TEnv::call_timeout)
			)
		  )
	{}

	call_record() = delete;

	call_record<TEnv> &operator=(const call_record<TEnv>&) = delete;

	bool arg_compare(const void *argp, uint16_t args)
	{
		if(args != arg.size())
			return false;

		return std::equal(
			arg.begin(),
			arg.end(),
			reinterpret_cast<const uint8_t*>(argp)
		);
	}

	void reset_expiry()
	{ expiry = clock::now()+std::chrono::milliseconds(static_cast<uint32_t>(call_timeout)); }
};

template<typename TEnv>
class incoming_call_handle;

template<typename TEnv>
struct incoming_call_record : public call_record<TEnv>
{
	typedef incoming_call_handle<TEnv>               incoming_call_handle_base;

	typedef std::function<void(incoming_call_handle_base&)> callback_type;


	template<typename... Targs>
	incoming_call_record(
		Targs... args
	)       
		: call_record<TEnv>(args...)
	{}

};

template<typename TEnv>
class outgoing_call_handle;

template<typename TEnv>
struct outgoing_call_record : public call_record<TEnv>
{
	typedef outgoing_call_handle<TEnv>               outgoing_call_handle_base;
	typedef typename TEnv::clock                     clock;
	typedef typename TEnv::time_point                time_point;

	typedef std::function<void(outgoing_call_handle_base)> callback_type;

	callback_type     callback;

	time_point        started;

	time_point        activated;

	volatile bool     parked;

	template<typename... Targs>
	outgoing_call_record(
		callback_type pcallback,
		bool pparked,
		Targs... args
	)       
		: call_record<TEnv>(args...)
		, callback(pcallback)
		, started(clock::now())
		, activated(pparked?time_point::min():clock::now())
		, parked(pparked)
	{}
};

}}
#endif
