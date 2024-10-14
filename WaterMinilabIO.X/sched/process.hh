#ifndef SCHED_PROCESS_H
# define SCHED_PROCESS_H
# include <thread>
# include <mutex>
# include <chrono>
# include <string>
# include "relay.hh"
namespace sched
{

struct process_abortion : public std::exception {};

class process
{
	std::thread *volatile _thread;

	std::mutex _lk;

	sched::relay _relay;

	inline void _run();

	virtual void init();

	virtual void exec() = 0;

	virtual void uninit();

public:

	virtual ~process();

	virtual const std::string &name();

	void start();

	void stop();

	inline void notify()
	{ _relay.notify(); }

	inline bool suspended()
	{ return _relay.suspend_count() > 0; }
	
	inline bool notified()
	{ return _relay.notified(); }

	bool is_current();

protected:

	process();

	template<typename... Ts>
	inline std::enable_if_t<(sizeof...(Ts) <= 1)> suspend(Ts&&... vs)
	{
		if(is_current())
			_relay.suspend(std::forward<Ts>(vs)...);
		else
			throw std::logic_error("suspend is only callable from within the process");
	}

	inline void abort()
	{
		if(is_current())
			throw process_abortion();
		else
			throw std::logic_error("abort is only callable from within the process");
	}
};

}
#endif
