#ifndef EARPC_PROCESS_FEEDBACK_H
# define EARPC_PROCESS_FEEDBACK_H
# include <journal.hh>
# include <iostream>
# include <cstdint>
# include <list>
# include <mutex>
# include <condition_variable>
namespace earpc {
namespace process
{
	template<typename TEnv>
	class feedback
	{
		typedef typename TEnv::call_id_type      call_id_type; 

		typedef typename TEnv::command_id_type   command_id_type; 

		typedef typename TEnv::header_type       header_type;

		typedef typename TEnv::address_type      address_type;

		template<typename T>
		using allocator = typename TEnv::template allocator<T>;

		constexpr static typename TEnv::connection_type  &conn = TEnv::conn;

		struct queue_record
		{
			address_type      ip;
			call_id_type      call_id;
			command_id_type   feedback;

			queue_record() = default;

			queue_record(
				address_type i,
				call_id_type cid,
				command_id_type fb
			)
				: ip(i)
				, call_id(cid)
				, feedback(fb)
			{}
		};

		typedef
		  std::list
		    < queue_record
		    , allocator<queue_record>
		    >
		  queue_type;

		static std::mutex               queue_lock;

		static std::mutex               suspend_lock;

		static std::condition_variable  suspend_cv;

		static queue_type               queue;

		static header_type              packet;

	public:
		static void start()
		{
			journal(journal::debug,"earpc.process.feedback") <<
				"initializing" <<
				journal::end;

			while(1)
			{
				queue_lock.lock();
				if(!queue.size())
				{
					queue_lock.unlock();

					journal(journal::trace,"earpc.process.feedback") << 
						"nothing to do; suspending until next notify" <<
						journal::end;

					std::unique_lock<std::mutex> ul(suspend_lock);
					suspend_cv.wait(ul);

					journal(journal::trace,"earpc.process.feedback") <<
						"resuming on notify" <<
						journal::end;

					continue;
				}

				for(
					typename queue_type::iterator i = queue.begin();
					i != queue.end();
				) {
					journal(journal::trace,"earpc.call") <<
						"call id: " << std::hex << i->call_id <<
						"; target: " << i->ip <<
						"; doing feedback send operation" <<
						journal::end;

					packet.call_id = i->call_id;
					packet.command_id = i->feedback;
					packet.checksum_create();
					conn.send(i->ip,std::vector<uint8_t,allocator<uint8_t>>((const uint8_t*)&packet,(const uint8_t*)(&packet+1)));
					queue.erase(i++);
				}
				queue_lock.unlock();
			}
			journal(journal::debug,"earpc.process.feedback") <<
				"uninitializing" <<
				journal::end;
		}

		static void notify(
			address_type ip,
			call_id_type call_id,
			command_id_type feedback
		)
		{
			queue_lock.lock();
			queue.emplace_back(ip,call_id,feedback);
			queue_lock.unlock();

			journal(journal::trace,"earpc.call") <<
				"call id: " << std::hex << call_id <<
				"; target: " << ip <<
				"; feedback queued for send" <<
				journal::end;

			std::lock_guard<std::mutex> lg(suspend_lock);
			suspend_cv.notify_one();
		}

		static void reroute(call_id_type call_id, address_type ip)
		{
			queue_lock.lock();
			for(auto &i : queue)
				if(i.call_id == call_id)
				{
					journal(journal::trace,"earpc.call") <<
						"call id: " << std::hex << i.call_id <<
						"; target: " << i.ip <<
						"; rerouting queued feedback to " << ip <<
						journal::end;

					i.ip = ip;
					queue_lock.unlock();
					return;
				}

			queue_lock.unlock();
			journal(journal::trace,"earpc.call") <<
				"call id: " << std::hex << call_id <<
				"; target: " << ip <<
				"; no queued feedback found for rerouting" <<
				journal::end;
		}
	};

	template<typename e>
	std::mutex feedback<e>::queue_lock;

	template<typename e>
	std::mutex feedback<e>::suspend_lock;

	template<typename e>
	std::condition_variable feedback<e>::suspend_cv;

	template<typename e>
	typename feedback<e>::queue_type feedback<e>::queue;

	template<typename e>
	typename feedback<e>::header_type feedback<e>::packet;
}}
#endif
