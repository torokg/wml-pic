#include <thread>
#include <condition_variable>
#include <system_error>
//////
namespace std
{

extern "C"
{
  static void*
  execute_native_thread_routine(void* __p)
  {
    thread::_State_ptr __t{ static_cast<thread::_State*>(__p) };
    __t->_M_run();
    return nullptr;
  }
}
    
thread::_State::~_State() {}

void
thread::_M_start_thread(std::unique_ptr<std::thread::_State> state, void(*)()) 
{
  if (!__gthread_active_p())
    {
#if __cpp_exceptions
      throw system_error(make_error_code(errc::operation_not_permitted),
                         "Enable multithreading to use std::thread");
#else
      __builtin_abort();
#endif
    }

  const int err = __gthread_create(&_M_id._M_thread,
                                   &execute_native_thread_routine,
                                   state.get());
  if (err)
    __throw_system_error(err);
  state.release();
}

void
thread::join()
{
  int __e = EINVAL;

  if (_M_id != id())
    __e = __gthread_join(_M_id._M_thread, 0);

  if (__e)
    __throw_system_error(__e);

  _M_id = id();
}




void
condition_variable::wait(unique_lock<mutex>& __lock) noexcept
{
    __gthread_cond_wait(&_M_cond, __lock.mutex()->native_handle());
}

void
condition_variable::notify_one() noexcept
{
    __gthread_cond_signal(&_M_cond);
}

void
condition_variable::notify_all() noexcept
{
    __gthread_cond_broadcast(&_M_cond);
}

condition_variable::condition_variable()
{
    __gthread_cond_init_function(&_M_cond);
}
condition_variable::~condition_variable()
{
    __gthread_cond_destroy(&_M_cond);
}



}

