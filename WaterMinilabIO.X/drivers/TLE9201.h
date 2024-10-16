#ifndef DRIVERS_TLE9201_HH
# define DRIVERS_TLE9201_HH

class TLE9201
{
    SPI &spi;
    GPIO_PIN pin_cs;
    
public:
    
    inline TLE9201(SPI &s, GPIO_PIN cs)
        : spi(s)
        , pin_cs(cs)
    {}
};
#endif