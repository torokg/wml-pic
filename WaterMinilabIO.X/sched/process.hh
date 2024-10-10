#ifndef SCHED_PROCESS_H
# define SCHED_PROCESS_H
# include <chrono>
# include <string>
# include <stdexcept>
# include <cstdint>
namespace sched
{

class process
{
    volatile uint8_t _state:2;
    volatile uint8_t _notified:1;

	virtual void init();

	virtual void exec() = 0;

	virtual void uninit();

public:

	virtual ~process();

	virtual const std::string &name();

	void start();

	void stop();

	void notify();

	bool suspended() const;
	
	bool notified() const;

	bool is_current() const;

    static void _serve();

protected:

	process();

    void suspend();

	/*template<typename... Ts>
	inline std::enable_if_t<(sizeof...(Ts) <= 1)> suspend(Ts&&... vs)
	{
		if(is_current())
			_suspend(std::forward<Ts>(vs)...);
		else
			throw std::logic_error("suspend is only callable from within the process");
	}*/
};

}
#endif
