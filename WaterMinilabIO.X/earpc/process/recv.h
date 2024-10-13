#ifndef EARPC_PROCESS_RECV_H
# define EARPC_PROCESS_RECV_H
# include <thread>
# include <functional>
namespace earpc {
namespace process
{
	template<typename TEnv>
	class recv
	{
		typedef typename TEnv::command_id_type           command_id_type;

		typedef typename TEnv::header_type               header_type;

		typedef typename TEnv::connection_type           connection_type;

		typedef typename TEnv::address_type              address_type;

		template<typename T>
		using allocator = typename TEnv::template allocator<T>;

		typedef typename TEnv::buf_command               buf_command;

		typedef typename TEnv::buf_incoming_call         buf_incoming_call;

		typedef typename TEnv::buf_outgoing_call         buf_outgoing_call;

		typedef typename TEnv::incoming_call_handle_base incoming_call_handle_base;

		typedef typename TEnv::outgoing_call_handle_base outgoing_call_handle_base;

		typedef typename TEnv::proc_feedback             proc_feedback;

		typedef typename TEnv::proc_send                 proc_send;

		typedef typename TEnv::proc_expiry               proc_expiry;

		typedef typename TEnv::proc_callback             proc_callback;

		static const command_id_type command_id_ack          = TEnv::command_id_ack;

		static const command_id_type command_id_nak          = TEnv::command_id_nak;

		static const command_id_type command_id_return       = TEnv::command_id_return;

		static const command_id_type command_id_exception    = TEnv::command_id_exception;

		constexpr static connection_type &conn                  = TEnv::conn;

		// Handle return value
		static void process_return(address_type ip, const std::vector<uint8_t,allocator<uint8_t>> &buffer)
		{
			const header_type &h = *reinterpret_cast<const header_type*>(buffer.data());

			proc_send::remove(h.call_id);

			buf_outgoing_call::lock();
			auto call = buf_outgoing_call::find(h.call_id);
			if(call != buf_outgoing_call::end())
			{
				if(call->parked)
				{
					journal(journal::critical,"earpc.call.outgoing") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << h.command_id <<
						"; target: " << ip <<
						"; return value on parked call; setting state to active" <<
						journal::end;

					call->parked = false;
				}

				if(call->ip != ip)
				{
					journal(journal::warning,"earpc.call.outgoing") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << h.command_id <<
						"; target: " << ip <<
						"; return value from different address; orig ip: " <<
						call->orig_ip <<
						journal::end;

					const address_type old_ip = call->ip;
					call->ip = ip;
					proc_send::reroute(h.call_id, ip);
					proc_feedback::reroute(h.call_id, ip);
					TEnv::on_outgoing_call_rerouted(old_ip,ip);
				}

				auto f = call->callback;

				const uint16_t ret_size = buffer.size()-sizeof(header_type);

				if(call->return_size == 0xffff || ret_size == call->return_size)
				{
					proc_feedback::notify(
						ip,
						h.call_id,
						command_id_ack
					);
					outgoing_call_handle_base handle(*call,&(&h)[1],ret_size,reason::success);

					journal(journal::trace,"earpc.call.outgoing") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << call->command_id <<
						"; target: " << ip <<
						"; successfully finished in " << std::dec <<
						::types::time::fmsec(handle.finished-handle.started) << " msec" <<
						journal::end;

					buf_outgoing_call::erase(call);
					TEnv::on_outgoing_call_finished(ip);
					buf_outgoing_call::unlock();

					proc_callback::notify(f,handle);
					return;
				}

				else
				{
					proc_feedback::notify(
						ip,
						h.call_id,
						command_id_nak
					);

					journal(journal::critical,"earpc.call.outgoing") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << call->command_id <<
						"; target: " << ip <<
						"; return value size mismatch" <<
						"; expected" << std::dec << call->return_size <<
						"; received " << ret_size <<
						journal::end;

					outgoing_call_handle_base handle(*call,0,0,reason::size_mismatch);
					buf_outgoing_call::erase(call);
					TEnv::on_outgoing_call_finished(ip);
					buf_outgoing_call::unlock();

					proc_callback::notify(f,handle);
					return;
				}
			}
			buf_outgoing_call::unlock();

			proc_feedback::notify(
				ip,
				h.call_id,
				command_id_nak
			);

			journal(journal::error,"earpc.call.outgoing") <<
				"call id: " << std::hex << h.call_id <<
				"; target: " << ip <<
				"; return value received for unknown call" <<
				journal::end;
		}

		// Handle incoming call
		static void process_call(address_type ip, const std::vector<uint8_t,allocator<uint8_t>> &buffer)
		{
			const header_type &h = *reinterpret_cast<const header_type*>(buffer.data());

			const uint16_t arg_size = buffer.size()-sizeof(header_type);
			
			// Process existing incoming call
			buf_incoming_call::lock();
			auto icall = buf_incoming_call::find(h.call_id);
			if(icall != buf_incoming_call::end())
			{
				if(icall->arg_compare(&(&h)[1],arg_size))
				{
					proc_feedback::notify(
						ip,
						h.call_id,
						command_id_ack
					);
					if(ip != icall->ip)
					{
						journal(journal::warning,"earpc.call.incoming") <<
							"call id: " << std::hex << h.call_id <<
							"; command: " << std::hex << h.command_id <<
							"; source: " << ip <<
							"; retransmitted call from different address; orig ip: " <<
							icall->orig_ip <<
							journal::end;
						icall->ip = ip;
						proc_send::reroute(h.call_id, ip);
						proc_feedback::reroute(h.call_id, ip);
					}
					else
						journal(journal::warning,"earpc.call.incoming") <<
							"call id: " << std::hex << h.call_id <<
							"; command: " << std::hex << h.command_id <<
							"; source: " << ip <<
							"; retransmitted call" <<
							journal::end;
				}

				else
				{
					proc_feedback::notify(
						ip,
						h.call_id,
						command_id_nak
					);
					journal(journal::error,"earpc.call.incoming") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << h.command_id <<
						"; source: " << ip <<
						"; duplicate call id" <<
						journal::end;
				}
				buf_incoming_call::unlock();
				return;
			}
			buf_incoming_call::unlock();

			// Process existing outgoing call
			buf_outgoing_call::lock();
			auto ocall = buf_outgoing_call::find(h.call_id);
			if(ocall != buf_outgoing_call::end())
			{
				buf_outgoing_call::unlock();
				proc_feedback::notify(
					ip,
					h.call_id,
					command_id_nak
				);
				journal(journal::error,"earpc.call.incoming") <<
					"call id: " << std::hex << h.call_id <<
					"; command: " << std::hex << h.command_id <<
					"; source: " << ip <<
					"; duplicate call id" <<
					journal::end;
				return;
			}
			buf_outgoing_call::unlock();


			buf_command::lock();
			typename buf_command::iterator command = buf_command::find(h.command_id);

			// Handle unknown command
			if(command == buf_command::end())
			{
				buf_command::unlock();
/*				proc_feedback::notify(
					ip,
					port,
					h.call_id,
					command_id_nak
				);
*/
				journal(journal::critical,"earpc.call.incoming") <<
					"call id: " << std::hex << h.call_id <<
					"; source: " << ip <<
					"; unknown command " << std::hex << h.command_id <<
					journal::end;
				return;
			}

			// Handle invalid argument size
			if(
				command->arg_size != arg_size &&
				command->arg_size != 0xffff
			)
			{ 
				buf_command::unlock();
				proc_feedback::notify(
					ip,
					h.call_id,
					command_id_nak
				);
				journal(journal::critical,"earpc.call.incoming") <<
					"call id: " << std::hex << h.call_id <<
					"; command: "<< std::hex << h.command_id <<
					"; source: " << ip <<
					"; argument size mismatch" <<
					"; expected " << std::dec << command->arg_size <<
					"; received " << arg_size <<
					journal::end;

				return;
			}

			// Serve new call
			auto f = command->callback;
			const uint16_t ret_size = command->ret_size;
			buf_command::unlock();

			proc_feedback::notify(
				ip,
				h.call_id,
				command_id_ack
			);

			buf_incoming_call::lock();
			auto call = buf_incoming_call::push(
				ip,
				h.command_id,
				h.call_id,
				&(&h)[1],
				arg_size,
				ret_size
			);
			incoming_call_handle_base handle(*call,reason::process);
			buf_incoming_call::unlock();

			proc_expiry::notify();
			journal(journal::trace,"earpc.call.incoming") << 
				"call id: " << std::hex << h.call_id <<
				"; command: " << std::hex << h.command_id <<
				"; source: " << ip <<
				"; serving call" <<
				journal::end;

			proc_callback::notify(f,handle);
		}

		static void process_ack(address_type ip, const std::vector<uint8_t,allocator<uint8_t>> &buffer)
		{
			const header_type &h = *reinterpret_cast<const header_type*>(buffer.data());

			proc_send::remove(h.call_id);

			// Process incoming calls
			buf_incoming_call::lock();
			auto icall = buf_incoming_call::find(h.call_id);
			if(icall != buf_incoming_call::end())
			{
				if(icall->ip != ip)
					journal(journal::warning,"earpc.call.incoming") <<
						"call id: " << std::hex << h.call_id <<
						"; command: " << std::hex << h.command_id <<
						"; source: " << ip <<
						"; return value accepted from different address; orig ip: " <<
						icall->orig_ip <<
						"; call finished" <<
						journal::end;
				else
					journal(journal::trace,"earpc.call.incoming") <<
						"call id: " << std::hex << icall->call_id <<
						"; command: " << std::hex << icall->command_id <<
						"; source: " << icall->ip <<
						"; return value accepted; call finished" <<
						journal::end;
				
				buf_incoming_call::erase(icall);
				buf_incoming_call::unlock();
				return;
			}
			buf_incoming_call::unlock();

			// Process outgoing calls
			buf_outgoing_call::lock();
			auto ocall = buf_outgoing_call::find(h.call_id);
			if(ocall == buf_outgoing_call::end())
				journal(journal::error,"earpc.call.outgoing") <<
					"call id: " << std::hex << h.call_id <<
					"; remote: " << ip <<
					"; ACK on unknown call id" <<
					journal::end;
			else
			{
				if(ocall->ip != ip)
				{
					journal(journal::trace,"earpc.call.outgoing") <<
						"call id: " << std::hex << ocall->call_id <<
						"; command: " << std::hex << ocall->command_id <<
						"; target: " << ip <<
						"; accepted from different address; orig ip: " <<
						ocall->orig_ip <<
						journal::end;

					const address_type old_ip = ocall->ip;
					ocall->ip = ip;
					proc_feedback::reroute(ocall->call_id,ip);
					TEnv::on_outgoing_call_rerouted(old_ip,ip);
				}
				else
					journal(journal::trace,"earpc.call.outgoing") <<
						"call id: " << std::hex << ocall->call_id <<
						"; command: " << std::hex << ocall->command_id <<
						"; target: " << ip <<
						"; accepted" <<
						journal::end;
			}
			buf_outgoing_call::unlock();
		}

		static void process_nak(address_type ip, const std::vector<uint8_t,allocator<uint8_t>> &buffer)
		{
			const header_type &h = *reinterpret_cast<const header_type*>(buffer.data());

			proc_send::remove(h.call_id);

			// Process incoming calls
			buf_incoming_call::lock();
			auto icall = buf_incoming_call::find(h.call_id);
			if(icall != buf_incoming_call::end())
			{
				if(icall->ip != ip)
				{
					journal(journal::critical,"earpc.call.incoming") <<
						"call id: " << std::hex << icall->call_id << 
						"; command: " << std::hex << icall->command_id <<
						"; source: " << ip <<
						"; return value rejected from different address; orig ip: " <<
						icall->orig_ip <<
						journal::end;
					icall->ip = ip;
				}
				else
					journal(journal::critical,"earpc.call.incoming") <<
						"call id: " << std::hex << icall->call_id << 
						"; command: " << std::hex << icall->command_id <<
						"; source: " << ip <<
						"; return value rejected" <<
						journal::end;

				buf_incoming_call::erase(icall);
				buf_incoming_call::unlock();
				return;
			}
			buf_incoming_call::unlock();
			buf_outgoing_call::lock();

			auto ocall = buf_outgoing_call::find(h.call_id);
			if(ocall != buf_outgoing_call::end())
			{
				if(ocall->ip != ip)
				{
					journal(journal::critical,"earpc.call.outgoing") <<
						"call id: " << std::hex << ocall->call_id << 
						"; command: " << std::hex << ocall->command_id <<
						"; target: " << ip <<
						"; call rejected from different address; orig ip: " <<
						ocall->orig_ip <<
						"; returning failure to API" <<
						journal::end;
					const address_type old_ip = ocall->ip;
					ocall->ip = ip;
					TEnv::on_outgoing_call_rerouted(old_ip,ip);
				}
				else
					journal(journal::critical,"earpc.call.outgoing") <<
						"call id: " << std::hex << ocall->call_id << 
						"; command: " << std::hex << ocall->command_id <<
						"; target: " << ip <<
						"; outgoing call rejected; returning failure" <<
						journal::end;

				outgoing_call_handle_base handle(*ocall,0,0,reason::rejected);
				auto f = ocall->callback;
				buf_outgoing_call::erase(ocall);
				TEnv::on_outgoing_call_finished(ip);
				buf_outgoing_call::unlock();
				proc_callback::notify(f,handle);
				return;
			}
			buf_outgoing_call::unlock();
			
			journal(journal::error,"earpc.call") <<
				"call id: " << std::hex << h.call_id <<
				"; source: " << ip <<
				"; NAK on unknown call id" <<
				journal::end;
		}

		static void process()
		{
			address_type ip;
			const auto buffer = conn.recv(ip);
			if(buffer.size() <= 0)
				return;

			if(buffer.size() < sizeof(header_type))
			{
				journal(journal::error,"earpc.call") <<
					"source: " << ip <<
					"; dropping due to packet size error" <<
					journal::end;
					
				return;
			}
            
            const header_type &h = *reinterpret_cast<const header_type*>(buffer.data());

			if(!h.checksum_verify())
			{
				journal(journal::error,"earpc.call") <<
					"source: " << ip <<
					"; dropping due to checksum error" <<
					journal::end;
				return;
			}

			switch(h.command_id)
			{
				case command_id_ack:
					process_ack(ip,buffer);
					break;
					
				case command_id_nak:
					process_nak(ip,buffer);
					break;

				case command_id_return:
					process_return(ip,buffer);
					break;

				case command_id_exception:
					proc_feedback::notify(
						ip,
						h.call_id,
						command_id_ack
					);
					break;

				default:
					process_call(ip,buffer);
					break;
			}
		}

	public:
		template<typename Treturn, typename Targ>
		struct incoming_call_handle : public incoming_call_handle_base
		{
			void respond(const Treturn &ret)
			{
				std::vector<uint8_t,allocator<uint8_t>> r;
				if(serializer<Treturn>::serialize(ret,r))
				{
					journal(journal::trace,"earpc.call.incoming") <<
						"call id: " << std::hex << incoming_call_handle_base::call_id <<
						"; command: " << std::hex << incoming_call_handle_base::command_id <<
						"; source: " <<  incoming_call_handle_base::ip <<
						"; sending return value" << journal::end;
					proc_send::notify(
						incoming_call_handle_base::ip,
						incoming_call_handle_base::call_id,
						command_id_return,
						r.data(),r.size()
					);
				}

				else
				{
					journal(journal::critical,"earpc.call.incoming") <<
						"call id: " << std::hex << incoming_call_handle_base::call_id <<
						"; command: " << std::hex << incoming_call_handle_base::command_id <<
						"; source: " <<  incoming_call_handle_base::ip <<
						"; unable to serialize return value; removing call" << journal::end;
					buf_incoming_call::lock();
					buf_incoming_call::remove(incoming_call_handle_base::call_id);
					buf_incoming_call::unlock();
				}
			}

			Targ value()
			{
				Targ r;
				if(serializer<Targ>::deserialize(r,incoming_call_handle_base::arg_bytes))
					journal(journal::trace,"earpc.call.incoming") <<
						"call id: " << std::hex << incoming_call_handle_base::call_id <<
						"; command: " << std::hex << incoming_call_handle_base::command_id <<
						"; source: " <<  incoming_call_handle_base::ip <<
						"; taking argument" << journal::end;
				else
					journal(journal::critical,"earpc.call.incoming") <<
						"call id: " << std::hex << incoming_call_handle_base::call_id <<
						"; command: " << std::hex << incoming_call_handle_base::command_id <<
						"; source: " <<  incoming_call_handle_base::ip <<
						"; unable to deserialize argument" << journal::end;

				return r;

			}

			template<typename T>
			incoming_call_handle(const T &call)
				: incoming_call_handle_base(call)
			{}
		};

		template<typename Treturn, typename Targ>
		struct outgoing_call_handle : public outgoing_call_handle_base
		{
			Treturn value()
			{
				if(outgoing_call_handle_base::reason)
				{
					journal(journal::critical,"earpc.call.outgoing") <<
						"call id: " << std::hex << outgoing_call_handle_base::call_id <<
						"; command: " << std::hex << outgoing_call_handle_base::command_id <<
						"; target: " <<  outgoing_call_handle_base::ip <<
						"; taking return value of unsuccessful call" << journal::end;

					return Treturn();
				}

				else
				{
					Treturn r;
					if(serializer<Treturn>::deserialize(r,outgoing_call_handle_base::ret_bytes))
						journal(journal::trace,"earpc.call.outgoing") <<
							"call id: " << std::hex << outgoing_call_handle_base::call_id <<
							"; command: " << std::hex << outgoing_call_handle_base::command_id <<
							"; target: " <<  outgoing_call_handle_base::ip <<
							"; taking return value" << journal::end;
					else
						journal(journal::critical,"earpc.call.outgoing") <<
							"call id: " << std::hex << outgoing_call_handle_base::call_id <<
							"; command: " << std::hex << outgoing_call_handle_base::command_id <<
							"; target: " <<  outgoing_call_handle_base::ip <<
							"; unable to deserialize return value" << journal::end;

					return r;
				}
			}
		};
        
        static void init()
        {}

		static void start()
		{
			journal(journal::debug,"earpc.process.recv") << "initializing" << journal::end;
			while(1)
				process();
    	}
	};
}}
#endif
