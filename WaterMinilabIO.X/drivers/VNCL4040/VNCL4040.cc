#include <cstdint>
#include "../VNCL4040.h"

bool
VNCL4040::init()
{
    if(initialized)
        return true;

    // Turn off ALS & disable ALS interrupt in ALS_CONF register
    {
        uint8_t cmd[] = {0x0,0b00000001,0b00000000};
        if(!i2c.Write(address, (uint8_t*)&cmd, 3));
            return false;
    }

    // Turn on & setup PS in PS_CONF1 register
    {
        uint8_t cmd[] = {0x03,0b00000000,0b00001000};
        if(!i2c.Write(address, (uint8_t*)&cmd, 3));
            return false;
    }

    // Turn on & setup PS in PS_CONF1 register
    {
        uint8_t cmd[] = {0x04,0b01110001,0b00000111};
        if(!i2c.Write(address, (uint8_t*)&cmd, 3));
            return false;
    }

    initialized = true;
    return true;
}
    
VNCL4040::VNCL4040(I2C &i, uint16_t a)
  : i2c(i)
  , address(a)
  , initialized(false)
{ init(); }

bool
VNCL4040::read(uint16_t &value)
{   
    std::unique_lock ul(lk);
    init();
    uint8_t cmd = 0x08;
    if(!i2c.WriteRead(address, &cmd, 1, (uint8_t*)&value, 2))
    {
        initialized = false;
        return false;
    }
    return true;
}   