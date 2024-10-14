#include <sstream>
#include <earpc/earpc.h>
#include "service.hh"
#include "spi_pkt_slave.hh"
#include "net/algorithm.h"
#include "px_int.h"
namespace io::host
{
    
typedef
 spi_pkt_slave
   < &SPI1_Read
   , &SPI1_Write
   , std::allocator
   >
hostio_raw;

typedef
 earpc::config
  < uint16_t		// command id
  , uint16_t		// call id
  , hostio_raw		// connection
  , uint8_t			// address
  , std::allocator
  >
 default_earpc_config;

typedef
  earpc::earpc<default_earpc_config>
  default_earpc;
    
    
static std::thread *proc_hostio = 0;

static std::thread *proc_recv_test = 0;

/*
static void recv_test_process()
{
    while(true)
    {
        uint8_t a = 0;
        auto y = hostio_raw::recv(a);
        
        if(y.size() < 6)
            continue;
        switch(y[0])
        {
            case (uint8_t)-1:
                //__builtin_software_breakpoint();
                break;
            case (uint8_t)-2:
                //__builtin_software_breakpoint();
                break;
            case (uint8_t)-3:
            {
                std::vector<uint8_t> x{0xff,0xff,y[2],y[3],0,0};
                uint16_t &checksum = reinterpret_cast<uint16_t*>(x.data())[2];
                checksum = 0;
                checksum = ::net::algorithm::hton(net::algorithm::checksum_finish(
                    net::algorithm::checksum_add(x.data(),6)
                ));
                hostio_raw::send(0,x);        
                break;
            }
            case (uint8_t)-4:
                __builtin_software_breakpoint();
                break;
        }
    }
}
*/

static size_t cnt = 0;

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
    //std::this_thread::sleep_for(100ms);
    
    test_lk = new std::mutex();
    test_cv = new std::condition_variable();
    
    while(true)
    {
        std::stringstream ss;
        ss << "Clock seconds: " << ::types::time::fsec(std::chrono::high_resolution_clock::now().time_since_epoch());
        default_earpc::call<bool,std::string>(
            0,0x1,ss.str(),
            [](auto r){
                volatile bool x = r.value();
                ++cnt;
                test_cv->notify_one();
            }
        );
        
        std::unique_lock ul(*test_lk);
        test_cv->wait(ul);
    }
    /*
    proc_recv_test = new std::thread(recv_test_process);
    uint16_t cnt = 0;
    //static std::condition_variable cv;
    //static std::mutex lk;
    while(true)
    {  
        
        
        ULONG avail = 0;
        tx_byte_pool_info_get(&posix_heap_byte_pool, 0, &avail, 0, 0, 0, 0);
        
            
        if(avail < 100000)
           __builtin_software_breakpoint();
        
        ++cnt;

        std::vector<uint8_t> x{0x1,0x0,(uint8_t)(cnt%0x100),(uint8_t)(cnt/0x100),0,0,'H','e','l','l','o'};
        uint16_t &checksum = reinterpret_cast<uint16_t*>(x.data())[2];
        checksum = 0;
        checksum = ::net::algorithm::hton(net::algorithm::checksum_finish(
            net::algorithm::checksum_add(x.data(),6)
        ));
        hostio_raw::send(0,x);
        
        //std::this_thread::sleep_for(10ms);
    }*/
}

bool service::init()
{
    //proc_hostio = new std::thread(&hostio_process);
    hostio_process();
    return true;
}

}
