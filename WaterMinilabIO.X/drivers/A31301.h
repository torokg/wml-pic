#ifndef DRIVERS_A31301_H
# define DRIVERS_A31301_H
# include <mutex>
# include <net/algorithm.h>
# include <drivers/i2c.hh>

class A31301
{
    I2C &i2c;
    uint16_t address;
    std::mutex lk;
    
public:
    
    struct result
    {
        uint16_t temp;
        uint16_t x;
        uint16_t y;
        uint16_t z;
    };
    
    A31301(I2C &i, uint16_t a)
      : i2c(i)
      , address(a)
    {}
      
    bool read(result &r)
    {
        uint8_t regaddr = 0x1c;
        if(!i2c.WriteRead(address, &regaddr, 1, (uint8_t*)&r, sizeof(result)))
            return false;
        
        for(uint16_t *p = (uint16_t*)&r; p != (uint16_t*)(&r+1); ++p)
            *p = net::algorithm::hton(*p);
        
        return true;
    } 
            
};
#endif