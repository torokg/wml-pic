#include <pthread.h>
#include "process.hh"
namespace sched
{

inline void process::_run()
{
	init();
	std::unique_lock ul(_lk);
	while(_thread)
	{
		ul.unlock();
		try
		{ exec(); }
		catch(const process_abortion&)
		{
			ul.lock();
			if(_thread)
			{
				std::thread *const t = _thread;
				_thread = nullptr;
				t->detach();
				delete t;
			}
			break;
		}
		ul.lock();
	}
	ul.unlock();
	uninit();
}

void process::init() {}

void process::uninit() {}

const std::string &process::name()
{
	static const std::string n = "sched::process";
	return n;
}

process::process()
	: _thread(nullptr)
{}

process::~process()
{ stop(); }

void process::start()
{
	std::unique_lock ul(_lk);
	if(_thread)
		return;
	_relay.reset();
	_thread = new std::thread(&sched::process::_run, this);
#ifdef __linux__
	pthread_setname_np(_thread->native_handle(), name().c_str());
#endif
}

bool process::is_current()
{
	std::unique_lock ulk(_lk);
	return _thread && (std::this_thread::get_id() == _thread->get_id());
}

void process::stop()
{
	std::unique_lock<std::mutex> ul(_lk);

	if(_thread)
	{
		std::thread *const t = _thread;
		_thread = nullptr;
		_lk.unlock();

		if(std::this_thread::get_id() == t->get_id())
		{
			try
			{ t->detach(); }
			catch(const std::system_error &ex){}
			delete t;
			throw process_abortion();
		}
		else
		{
			notify();
			try
			{ t->join(); }
			catch(const std::system_error &ex)
			{
				try
				{ t->detach(); }
				catch(const std::system_error &ex){}
			}
			delete t;
		}
	}
}

}
