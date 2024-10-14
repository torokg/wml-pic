#include <sstream>
#include "service.hh"
#include "px_int.h"

namespace io::host
{
    
static std::thread *proc_hostio = 0;

static std::mutex *test_lk;
static std::condition_variable *test_cv;

static void hostio_process()
{
    using namespace std::literals::chrono_literals;
    
    SPI1_Initialize();
    
    if(!hostio_raw::init())
    {
        __builtin_software_breakpoint();
    }
    
    default_earpc::init();
    
    test_lk = new std::mutex();
    test_cv = new std::condition_variable();
    
    auto t0 = std::chrono::high_resolution_clock::now();
    while(true)
    {
        auto t = std::chrono::high_resolution_clock::now();
        
        if(t >= t0)
        {
	     ULONG avail = 0;
	     tx_byte_pool_info_get(&posix_heap_byte_pool, 0, &avail, 0, 0, 0, 0);
            std::stringstream ss;
            ss << "Clock: " << ::types::time::fsec(t.time_since_epoch()) << "; memory: " << avail << " bytes available";
            default_earpc::call<bool,std::string>(
                0,0x1,ss.str(),
                [](auto r){
                    volatile bool x = r.value();
                    test_cv->notify_one();
                }
            );

            std::unique_lock ul(*test_lk);
            test_cv->wait(ul);
        }
        t0 = t + std::chrono::seconds(3);
        std::this_thread::sleep_until(t0);
    }
}

bool service::init()
{
    proc_hostio = new std::thread(&hostio_process);
    return true;
}

}
