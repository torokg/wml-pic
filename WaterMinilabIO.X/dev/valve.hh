#ifndef DEV_VALVE_HH
# define DEV_VALVE_HH
# include <drivers/MCP23S17.h>
namespace dev
{

class valve
{
    MCP23S17 &portexp;
    uint8_t   pin;
    
public:
    
    inline valve(MCP23S17 &pexp, uint8_t p)
        : portexp(pexp)
        , pin(p)
    {}
        
    inline void operator()(bool v)
    { portexp.pin_value(pin,v); }
};


}
#endif