#ifndef WIC_EARPC_BUFFER_INCOMING_CALL_H
# define WIC_EARPC_BUFFER_INCOMING_CALL_H
# include <earpc/buffer/buffer.h>
# include <list>
# include <chrono>
# include <mutex>
namespace wic {
namespace earpc {
namespace buffer
{
	template<typename TEnv>
	using incoming_call_base = buffer<TEnv,typename TEnv::incoming_call_record>;

	template<typename TEnv>
	class incoming_call : public incoming_call_base<TEnv>
	{
		typedef incoming_call_base<TEnv> base;

		typedef typename base::iterator iterator;

	public:
		static iterator find_first_expired()
		{
			for(auto i = base::begin(); i != base::end(); ++i)
				if(i->expiry <= base::clock::now())
					return i;
			return base::end();
		}

		static iterator find_next_expiring()
		{
			auto r = base::begin();
			for(auto i = base::begin(); i != base::end(); ++i)
				if(i->expiry < r->expiry)
					r = i;
			return r;
		}
	};
}}}
#endif
