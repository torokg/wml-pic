#ifndef WIC_EARPC_BUFFER_BUFFER_H
# define WIC_EARPC_BUFFER_BUFFER_H
# include <list>
# include <mutex>
# include <earpc/types/call_record.h>
namespace wic {
namespace earpc {
namespace buffer
{
	template<typename TEnv, typename TRecord>
	struct buffer 
	{
		typedef typename TEnv::clock                clock;

		typedef typename TEnv::time_point           time_point;

		typedef typename TEnv::call_id_type         call_id_type;
		
		typedef TRecord                             record_type;

		typedef typename record_type::callback_type callback_type;

		template<typename T>
		using allocator = typename TEnv::template allocator<T>;

		typedef
		  std::list
		    < record_type
		    , allocator<record_type>
		    >
		  container_type;

	private:
		static container_type                       container;

		static std::mutex                           mutex;

	public:
		typedef typename container_type::iterator   iterator;

		static void lock()
		{ mutex.lock(); }

		static void unlock()
		{ mutex.unlock(); }

		static iterator begin()
		{ return container.begin(); }

		static iterator end()
		{ return container.end(); }

		static iterator find(call_id_type call_id)
		{
			for(auto i = begin(); i != end(); ++i)
				if(i->call_id == call_id)
					return i;
			return end();
		}

		template<typename... Targ>
		static iterator push(Targ... args)
		{ return container.emplace(begin(),args...); }

		static void erase(iterator i)
		{ container.erase(i); }

		static bool remove(call_id_type cid)
		{
			auto i = find(cid);
			if(i == end())
				return false;
			erase(i);
			return true;
		}
	};

	template<typename e,typename r>
	typename buffer<e,r>::container_type buffer<e,r>::container;

	template<typename e,typename r>
	std::mutex buffer<e,r>::mutex;
}}}
#endif
