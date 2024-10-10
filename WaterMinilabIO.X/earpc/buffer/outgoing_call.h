#ifndef WIC_EARPC_BUFFER_OUTGOING_CALL_H
# define WIC_EARPC_BUFFER_OUTGOING_CALL_H
# include <earpc/buffer/buffer.h>
# include <list>
# include <chrono>
# include <mutex>
namespace wic {
namespace earpc {
namespace buffer
{
	template<typename TEnv>
	using outgoing_call_base = buffer<TEnv,typename TEnv::outgoing_call_record>;

	template<typename TEnv>
	class outgoing_call : public outgoing_call_base<TEnv>
	{
		typedef outgoing_call_base<TEnv> base;

		typedef typename TEnv::address_type address_type;

		typedef typename base::iterator iterator;

	public:
		static iterator find_first_expired()
		{
			for(auto i = base::begin(); i != base::end(); ++i)
				if(!i->parked && i->expiry <= base::clock::now())
					return i;
			return base::end();
		}

		static iterator find_next_expiring()
		{
			auto r = base::begin();
			for(auto i = base::begin(); i != base::end(); ++i)
				if(!i->parked && i->expiry < r->expiry)
					r = i;
			return r;
		}

		static iterator find_first_parked_to(address_type ip)
		{
			for(auto i = base::begin(); i != base::end(); ++i)
				if(i->parked && i->ip == ip)
					return i;
			return base::end();
		}

		static iterator find_first_active_to(address_type ip)
		{
			for(auto i = base::begin(); i != base::end(); ++i)
				if(!i->parked && i->ip == ip)
					return i;
			return base::end();
		}
	};
}}}
#endif
