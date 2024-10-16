#ifndef DRIVERS_TLE9201_HH
# define DRIVERS_TLE9201_HH
# include <cstdint>
# include <drivers/spi.hh>

class TLE9201
{
    constexpr static inline uint8_t spwm_mask = (1<<1);
    constexpr static inline uint8_t sdir_mask = (1<<1);
    constexpr static inline uint8_t sen_mask  = (1<<2);
    
    SPI &spi;
    GPIO_PIN pin_cs;
    std::mutex lk;
    
    volatile uint8_t ctl_reg;
    
    void _write_ctl();
        
public:
    
    TLE9201(SPI &s, GPIO_PIN cs);
    
    void start(bool dir);
    
    void stop();
};
#endif