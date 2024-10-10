#ifndef SCHED_RELAY_HH
# define SCHED_RELAY_HH
# include <chrono>
# include <mutex>
# include <condition_variable>
namespace sched
{

class relay
{
	typedef std::unique_lock<std::mutex> ul_t;
	typedef std::lock_guard<std::mutex>  lg_t;

	std::mutex              lk;
	std::condition_variable cv;
	volatile size_t         sc;
	volatile bool           en;

public:
	inline relay()
		: sc(0)
		, en(false)
	{};

	~relay()
	{
		ul_t ul(lk);
		if(sc)
		{
			//TODO: throw sched::invalid_state ?? or something...
			abort();
		}
	}

	inline void suspend()
	{
		ul_t ul(lk);
		if(!en)
		{
			++sc;
			do cv.wait(ul);
			while(!en);
			--sc;
		}
		en = false;
	}

	template<typename Trep, typename Tratio>
	inline void suspend(const std::chrono::duration<Trep,Tratio> &d)
	{
		ul_t ul(lk);
		if(!en)
		{
			++sc;
			std::cv_status s;
			do s = cv.wait_for(ul,d);
			while(!en && (s == std::cv_status::no_timeout));
			--sc;
		}
		en = false;
	}

	template<typename Tclock, typename Trep>
	inline void suspend(const std::chrono::time_point<Tclock,Trep> &t)
	{
		ul_t ul(lk);
		if(!en)
		{
			++sc;
			std::cv_status s;
			do s = cv.wait_until(ul,t);
			while(!en && (s == std::cv_status::no_timeout));
			--sc;
		}
		en = false;
	}

	inline void notify()
	{
		lg_t lg(lk);
		en = true;
		cv.notify_all();
	}

	inline void reset()
	{
		lg_t lg(lk);
		en = false;
	}

	inline bool is_notified() const
	{ return en; }

	inline size_t suspend_count() const
	{ return sc; }
};

}
#endif
