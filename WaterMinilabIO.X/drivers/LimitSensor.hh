#ifndef DRIVERS_LIMIT_SENSOR_HH
# define DRIVERS_LIMIT_SENSOR_HH
# include <drivers/A31301.h>

class LimitSensor
    : public A31301
{
    int16_t limit[2];
    uint8_t  axis;
    bool     invert;
    constexpr static inline int measures = 1;
    
public:
    
    inline LimitSensor(I2C &i, uint16_t a, uint8_t ax, int16_t liml, int16_t limh, bool inv)
        : A31301(i,a)
        , limit{liml,limh}
        , axis(ax)
        , invert(inv)
    {}
        
    inline int16_t raw()
    {
        int acc = 0;
        
        for(int i = 0; i < measures; ++i)
        {
            result r;
            read(r);
            acc += (&r.x)[axis];
            //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return (int16_t)(acc/measures);
    }
        
    inline int operator()()
    {
        const auto v = raw();
        const auto inv = (invert?-1:1);
        if (v < limit[0])
            return (v-limit[0])*inv;
        if(v > limit[1])
            return (v-limit[1])*inv;
        
        return 0;
        
    }
};
#endif