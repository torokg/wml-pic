#include <thread>
#include <mutex>
#include <condition_variable>
#include <io/host/service.hh>

void init()
{
    if(!io::host::service::init())
    {
        __builtin_software_breakpoint();
    }
    
    std::mutex lk;
    std::condition_variable cv;

    /*std::thread
        a([&]{
            while(true)
            {
                std::unique_lock ul(lk);
                char *x = new char[8];
                cv.wait(ul);
                ul.unlock();
                cv.notify_one();
                delete x;
            }
        }),

        b([&]{
            while(true)
            {
                char *x = new char[8];
                cv.notify_one();
                std::unique_lock ul(lk);
                cv.wait(ul);
                delete x;
            }
        });*/
    
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1200));
    }
}
