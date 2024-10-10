namespace earpc {

template<
	typename TcommandId,
	typename TcallId,
	typename Tconnection,
	typename Taddress,
	template<typename>
	typename Tallocator,
	auto... tConnArgs
>
struct config
{
	typedef TcommandId               cfg_command_id_type;
                                  
	typedef TcallId                  cfg_call_id_type;

	typedef Tconnection              cfg_connection_type;

	typedef Taddress                 cfg_address_type;

	static const cfg_command_id_type cfg_command_id_ack       = -1;

	static const cfg_command_id_type cfg_command_id_nak       = -2;

	static const cfg_command_id_type cfg_command_id_return    = -3;

	static const cfg_command_id_type cfg_command_id_exception = -4;

	static const uint32_t            cfg_call_timeout         = 3200;

	static Tconnection               cfg_connection;

	template<typename T>
	using cfg_allocator = Tallocator<T>;
};

template<typename a, typename b, typename c, typename d, template<typename> typename e, auto... args>
c config<a,b,c,d,e,args...>::cfg_connection(args...);

}
