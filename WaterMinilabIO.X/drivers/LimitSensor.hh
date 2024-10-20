#ifndef DRIVERS_LIMIT_SENSOR_HH
# define DRIVERS_LIMIT_SENSOR_HH
# include <drivers/A31301.h>

class LimitSensor
    : public A31301
{
    uint16_t limit;
    uint8_t  axis;
    
public:
    
    inline LimitSensor(I2C &i, uint16_t a, uint8_t ax, uint16_t lim)
        : A31301(i,a)
        , limit(lim)
        , axis(ax)
    {}
        
    inline bool operator()()
    { 
        result r;
        read(r);
        
        return abs((&r.x)[axis]) > limit;
    }
};
#endif