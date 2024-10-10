#ifndef EARPC_PROCESS_EXPIRY_H
# define EARPC_PROCESS_EXPIRY_H
# include <journal.hh>
# include <iostream>
# include <cstdint>
# include <list>
# include <mutex>
# include <condition_variable>
# include <types/time.hh>
#ifdef _MSC_VER
	#undef max
	#undef min
#endif
extern "C" long long debug_long0, debug_long1;
namespace earpc::process
{
	template<typename TEnv>
	class expiry 
	{
		typedef typename TEnv::clock             clock;

		typedef typename clock::time_point       time_point;

		typedef typename TEnv::buf_outgoing_call buf_outgoing_call;

		typedef typename TEnv::buf_incoming_call buf_incoming_call;

		typedef typename TEnv::outgoing_call_handle_base outgoing_call_handle_base;

		typedef typename TEnv::proc_send         proc_send;

		typedef typename TEnv::proc_callback     proc_callback;


		static std::mutex                        suspend_lock;

		static std::condition_variable           suspend_cv;

	public:
		static void start()
		{
			journal(journal::debug,"earpc.process.expiry") << "initializing" << journal::end;

			while(1)
			{
				buf_outgoing_call::lock();

				time_point ns = time_point::max();
				
				// Process outgoing calls
				for(
					auto call = buf_outgoing_call::find_first_expired();
					call != buf_outgoing_call::end();
					call = buf_outgoing_call::find_first_expired()
				) {
					proc_send::remove(call->call_id);
					journal(journal::error,"earpc.call.outgoing") << 
						"call id: " << std::hex << call->call_id <<
						"; command: " << std::hex << call->command_id <<
						"; target: " << call->ip <<						
						"; call expired" <<
						journal::end;
					auto f = call->callback;
					outgoing_call_handle_base handle(*call,0,0,reason::expired);
					buf_outgoing_call::erase(call);
					TEnv::on_outgoing_call_finished(handle.ip);
					buf_outgoing_call::unlock();

					proc_callback::notify(f,handle);
					buf_outgoing_call::lock();
				}
				{
					auto call = buf_outgoing_call::find_next_expiring();
					if(call != buf_outgoing_call::end())
						ns = std::min(ns,call->expiry);
				}
				buf_outgoing_call::unlock();

				// Process incoming calls
				buf_incoming_call::lock();
				for(
					auto call = buf_incoming_call::find_first_expired();
					call != buf_incoming_call::end();
					call = buf_incoming_call::find_first_expired()
				) {
					proc_send::remove(call->call_id);
					journal(journal::error,"earpc.call.incoming") << 
						"call id: " << std::hex << call->call_id <<
						"; command: " << std::hex << call->command_id <<
						"; target: " << call->ip <<						
						"; call expired" <<
						journal::end;

					buf_incoming_call::erase(call);
					buf_incoming_call::unlock();
					buf_incoming_call::lock();
				}
				{
					auto call = buf_incoming_call::find_next_expiring();
					if(call != buf_incoming_call::end())
						ns = std::min(ns,call->expiry);
				}
				buf_incoming_call::unlock();

				// Suspend
				if(ns == time_point::max())
				{
					journal(journal::trace,"earpc.process.expiry") << 
						"nothing to do; suspending until next notify" <<
						journal::end;

					std::unique_lock<std::mutex> ul(suspend_lock);
					suspend_cv.wait(ul);
					journal(journal::trace,"earpc.process.expiry") << "resuming" << journal::end;
				}

				else
				{
					journal(journal::trace,"earpc.process.expiry") << 
						"nothing to do; suspending for " <<
						std::dec << ::types::time::msec(ns-clock::now()) << " msec" <<
						journal::end;

					std::unique_lock<std::mutex> ul(suspend_lock);
					suspend_cv.wait_until(ul,ns);
					journal(journal::trace,"earpc.process.expiry") << "resuming" << journal::end;
				}
			}
			journal(journal::debug,"earpc.process.expiry") << "uninitializing" << journal::end;
		}

		static void notify()
		{
			std::lock_guard<std::mutex> lg(suspend_lock);
			suspend_cv.notify_one();
		}

	};

	template<typename e>
	std::mutex expiry<e>::suspend_lock;

	template<typename e>
	std::condition_variable expiry<e>::suspend_cv;
}
#endif
