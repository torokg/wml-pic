#include <earpc/earpc.h>
#include "service.hh"
#include "spi_pkt_slave.hh"
#include "net/algorithm.h"
#include "px_int.h"
namespace io::host
{
    
static inline void hostio_clkrq_set()
{ SPI0_INT_Set(); }

static inline void hostio_clkrq_clr()
{ SPI0_INT_Clear(); }
    
    
static std::thread *proc_hostio = 0;

static void hostio_process()
{
    
    SPI1_Initialize();
/*
    typedef
     spi_pkt_slave
       < &SPI1_Read
       , &SPI1_Write
       , &SPI1_CallbackRegister
       , &SPI1_ErrorGet
       , &SPI1_IsBusy
       , &SPI1_WriteComplete
       , &hostio_clkrq_set
       , &hostio_clkrq_clr
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
    
    
    if(!hostio_raw::init())
    {
        __builtin_software_breakpoint();
    }
    */
    //default_earpc::init();
    
    uint8_t cnt = 0;
    //static std::condition_variable cv;
    //static std::mutex lk;
    while(true)
    {  
        using namespace std::literals::chrono_literals;
        
        ++cnt;
        ULONG avail = 0;
        tx_byte_pool_info_get(&posix_heap_byte_pool, 0, &avail, 
                    0, 0, 
                    0, 0);
        std::vector<uint8_t> x{0xaa,11,0x1,0x0,cnt,cnt,0,0,'H','e','l','l','o',0x55};
        if(avail < 100000)
            __builtin_software_breakpoint();
		uint16_t &checksum = reinterpret_cast<uint16_t*>(x.data())[3];
        checksum = 0;
		checksum = ::net::algorithm::hton(net::algorithm::checksum_finish(
			net::algorithm::checksum_add(x.data()+2,6)
		));

        SPI1_Write(x.data(),x.size());
        
        //hostio_raw::send(0,x);
                
        //std::unique_lock ul(lk);
        
        //default_earpc::call<bool,std::string>(0,0x1,"Hello",
                
        //        [](auto){}
        /*[](auto){
            std::unique_lock ul(lk);
            cv.notify_all();
        }*/
        //);
        
        //cv.wait(ul);
        std::this_thread::sleep_for(10000ms);
    }
}

bool service::init()
{
    proc_hostio = new std::thread(&hostio_process);
    return true;
}

}
