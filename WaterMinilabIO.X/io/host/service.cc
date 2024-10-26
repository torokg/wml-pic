#include <sstream>
#include "service.hh"

namespace io::host
{

bool service::init()
{
    SPI1_Initialize();
    
    if(!hostio_raw::init())
    {
        __builtin_software_breakpoint();
        return false;
    }
    
    default_earpc::init();
    return true;
}

}
