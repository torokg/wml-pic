#ifndef EARPC_PROCESS_SEND_H
# define EARPC_PROCESS_SEND_H
# include <cstdint>
# include <cstring>
# include <list>
# include <forward_list>
# include <mutex>
# include <condition_variable>
# include <types/time.hh>
#ifdef _MSC_VER
	#undef max
	#undef min
#endif
namespace earpc {
namespace process
{
	template<typename TEnv>
	class send
	{
		typedef typename TEnv::call_id_type      call_id_type;

		typedef typename TEnv::command_id_type   command_id_type;

		typedef typename TEnv::header_type       header_type;

		typedef typename TEnv::connection_type   connection_type;

		typedef typename TEnv::address_type      address_type;

		template<typename T>
		using allocator = typename TEnv::template allocator<T>;

		typedef typename TEnv::outgoing_call_record outgoing_call_record;

		typedef typename TEnv::clock             clock;

		typedef typename clock::time_point       time_point;
        
        constexpr static std::chrono::milliseconds resend_interval{TEnv::resend_interval};


		static std::mutex               &queue_lock;

		static std::mutex               &suspend_lock;

		static std::condition_variable  &suspend_cv;

		constexpr static connection_type &conn = TEnv::conn;

		struct packet
		{
			const address_type ip;
			uint8_t *buf;
			const uint16_t size;
			packet(
				address_type pip,
				uint8_t *pbuf,
				uint16_t psize
			)
				: buf(pbuf)
				, ip(pip)
				, size(psize)
			{}
		};

		struct queue_record
			: allocator<uint8_t>
		{
			address_type ip;

			call_id_type call_id;

			command_id_type command_id;

			uint8_t *buffer;

			uint16_t size;

			time_point resend_time;

			queue_record(const queue_record &that)
				: ip(that.ip)
				, call_id(that.call_id)
				, command_id(that.command_id)
				, buffer(this->allocate(that.size))
				, size(that.size)
				, resend_time(that.resend_time)
			{ memcpy(buffer,that.buffer,size); }

			queue_record()
				: buffer(0)
			{}

			queue_record(
				address_type i,
				call_id_type cid,
				command_id_type cmd,
				const void *b,
				uint16_t s
			)
				: ip(i)
				, call_id(cid)
				, command_id(cmd)
				, buffer(this->allocate(s))
				, size(s)
				, resend_time(time_point::min())
			{ memcpy(buffer,b,s); }

			~queue_record()
			{ this->deallocate(buffer,size); }

			void send()
			{
				const uint16_t buf_size = sizeof(header_type) + size;
				auto buf = std::vector<uint8_t,allocator<uint8_t>>(buf_size,0);
				
				header_type &h = *reinterpret_cast<header_type*>(buf.data());
				h.call_id = call_id;
				h.command_id = command_id;
				h.checksum_create();
				memcpy(buf.data()+sizeof(header_type),buffer,size);
				conn.send(ip, buf);
			}
			
		};

		typedef
		  std::list
		    < queue_record
		    , allocator<queue_record>
		    >
		  queue_type;

		static queue_type               &queue;

		static volatile bool            notified;

	public:
        static void init()
        {
            new(&queue_lock) std::mutex();
            new(&suspend_lock) std::mutex();
            new(&suspend_cv) std::condition_variable();
            new(&queue) queue_type();
        }
        
		static void start()
		{
			journal(journal::debug,"earpc.process.send") << "initializing" << journal::end;
			notified = false;

			while(1)
			{
				time_point ns = time_point::max();
				
                const auto t0 = clock::now();
				queue_lock.lock();
				for(auto &i : queue)
				{
					if(i.resend_time > clock::now())
					{
						if(ns > i.resend_time)
							ns = i.resend_time;
						continue;
					}

					i.resend_time = clock::now() + resend_interval;
					if(ns > i.resend_time)
						ns = i.resend_time;

					journal(journal::trace,"earpc.call") <<
						"call id: " << std::hex << i.call_id <<
						"; command: " << std::hex << i.command_id <<
						"; target: " << i.ip <<
						"; doing data send operation" <<
						journal::end;

					i.send();
				}
				queue_lock.unlock();

				if(ns == time_point::max())
				{
					journal(journal::trace,"earpc.process.send") <<
						"nothing to do; suspending until next notify" <<
						journal::end;

					std::unique_lock<std::mutex> ul(suspend_lock);
					if(!notified)
						suspend_cv.wait(ul);
					else
						notified = false;
				}

				else
				{
					journal(journal::trace,"earpc.process.send") <<
						"nothing to do; suspending for " << std::dec <<
						::types::time::msec(ns-clock::now()) <<
						" msec" << journal::end;

					std::unique_lock<std::mutex> ul(suspend_lock);
					if(!notified)
						suspend_cv.wait_until(ul,ns);
					else
						notified = false;
				}
				journal(journal::trace,"earpc.process.send") << "resuming" << journal::end;
			}

			journal(journal::debug,"earpc.process.send") << "uninitializing" << journal::end;
		}

		static void notify(
			address_type ip,
			call_id_type call_id,
			command_id_type command_id,
			const void *buffer,
			uint16_t size
		)
		{
			queue_lock.lock();
			queue.emplace_back(ip,call_id,command_id,buffer,size);
			queue_lock.unlock();

			journal(journal::trace,"earpc.call") <<
				"call id: " << std::hex << call_id <<
				"; command: " << std::hex << command_id <<
				"; target: " << ip <<
				"; " << std::dec << size <<
				" bytes of data queued for send" <<
				journal::end;

			std::lock_guard<std::mutex> lg(suspend_lock);
			notified = true;
			suspend_cv.notify_one();
		}

		static void notify(const outgoing_call_record &rec)
		{ notify(rec.ip,rec.call_id,rec.command_id,rec.arg.data(),rec.arg.size()); }

		static void reroute(call_id_type call_id, address_type ip)
		{
			queue_lock.lock();
			for(auto &i : queue)
				if(i.call_id == call_id)
				{
					journal(journal::trace,"earpc.call") <<
						"call id: " << std::hex << i.call_id <<
						"; command: " << std::hex << i.command_id <<
						"; target: " << i.ip <<
						"; rerouting queued data to " << ip <<
						journal::end;
					i.ip = ip;
					queue_lock.unlock();
					return;
				}
			queue_lock.unlock();

			journal(journal::trace,"earpc.call") <<
				"call id: " << std::hex << call_id <<
				"; target: " << ip <<
				"; no queued data found for rerouting" <<
				journal::end;
		}

		static void remove(call_id_type call_id)
		{
			queue_lock.lock();
			for(auto i = queue.begin(); i != queue.end(); ++i)
				if(i->call_id == call_id)
				{
					journal(journal::trace,"earpc.call") <<
						"call id: " << std::hex << call_id <<
						"; command: " << std::hex << i->command_id <<
						"; target: " << i->ip <<
						"; removing queued data" <<
						journal::end;

					queue.erase(i);
					queue_lock.unlock();
					return;
				}
			queue_lock.unlock();
			
			journal(journal::trace,"earpc.call") <<
				"call id: " << std::hex << call_id <<
				"; no queued data found for removal" <<
				journal::end;
		}
	};

	template<typename e>
	std::mutex &send<e>::queue_lock = *(std::mutex*)malloc(sizeof(std::mutex));

	template<typename e>
	std::mutex &send<e>::suspend_lock = *(std::mutex*)malloc(sizeof(std::mutex));

	template<typename e>
	std::condition_variable &send<e>::suspend_cv = *(std::condition_variable*)malloc(sizeof(std::condition_variable));

	template<typename e>
	typename send<e>::queue_type &send<e>::queue = *(typename send<e>::queue_type*)malloc(sizeof(typename send<e>::queue_type));

	template<typename e>
	volatile bool send<e>::notified = false;
}}
#endif
