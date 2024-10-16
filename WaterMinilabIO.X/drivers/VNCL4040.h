#ifndef DRIVERS_VNCL4040_H
# define DRIVERS_VNCL4040_H
# include <mutex>
# include <drivers/i2c.hh>
class VNCL4040
{
    std::mutex lk;
    I2C &i2c;
    const uint16_t address;
    volatile bool initialized;    
    bool init();
    
public:
    
    VNCL4040(I2C &i, uint16_t a);
      
    bool read(uint16_t &value);
};
#endif