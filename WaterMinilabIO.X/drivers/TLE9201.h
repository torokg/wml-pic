#ifndef DRIVERS_TLE9201_HH
# define DRIVERS_TLE9201_HH
# include <cstdint>
# include <drivers/spi.hh>

class TLE9201
{
    constexpr static inline uint8_t ctl_spwm_mask = (1<<0);
    constexpr static inline uint8_t ctl_sdir_mask = (1<<1);
    constexpr static inline uint8_t ctl_sen_mask  = (1<<2);
    
    constexpr static inline uint8_t dia_tv_mask  = (1<<2);
    constexpr static inline uint8_t dia_en_mask  = (1<<7);
    
    SPI &spi;
    GPIO_PIN pin_cs;
    std::mutex lk;
    
    volatile uint8_t ctl_reg;
    volatile uint8_t dia_reg;
    
    bool _tx();
        
public:
    
    TLE9201(SPI &s, GPIO_PIN cs);
    
    bool start(bool dir);
    
    bool stop();
};
#endif