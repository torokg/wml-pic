/* 
 * File:   service.hh
 * Author: root
 *
 * Created on May 19, 2024, 2:17 PM
 */

#ifndef IO_HOST_SERVICE_HH
# define IO_HOST_SERVICE_HH
# include <cstdint>
# include <sstream>
# include "spi_pkt_slave.hh"
# include "net/algorithm.h"
# include <earpc/earpc.h>


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
  , hostio_raw      // connection
  , uint8_t			// address
  , std::allocator
  >
 default_earpc_config;

typedef
  earpc::earpc<default_earpc_config>
  default_earpc;


    class service
    {
    public:
        service() = delete;
        static bool init();
        
        template<typename... Ts>
        static void log(Ts&&... vs)
        {
            std::stringstream ss;
            
            ((ss << std::forward<Ts>(vs)), ... );
            
            std::mutex lk;
            std::condition_variable cv;
            
            default_earpc::call<bool,std::string>(
                0,0x1,ss.str(),
                [&cv](auto r){
                    volatile bool x = r.value();
                    cv.notify_one();
                }
            );
            std::unique_lock ul(lk);
            cv.wait(ul);
        }
    };
    
    template<typename... Ts>
    inline void log(Ts&&... vs)
    { service::log(std::forward<Ts>(vs)...); }

}

#endif	/* SERVICE_HH */

