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
        int16_t temp;
        int16_t x;
        int16_t y;
        int16_t z;
    };
    
    A31301(I2C &i, uint16_t a);
      
    bool read(result &r);
};
#endif