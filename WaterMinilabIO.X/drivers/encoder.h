#ifndef DRIVERS_ENCODER_H
# define DRIVERS_ENCODER_H
# include <array>
# include "definitions.h"

class Encoder
{
    GPIO_PIN pin_en;
    GPIO_PIN pin_a;
    GPIO_PIN pin_b;
    GPIO_PIN pin_z;

public:
    
    std::array<volatile uint8_t,4> state;
    volatile unsigned statep;
    
    volatile int32_t position;
    const int32_t direction;
    const bool intpin;
    
    constexpr Encoder(GPIO_PIN en, GPIO_PIN a, GPIO_PIN b, GPIO_PIN z, bool ip, int32_t dir)
        : pin_en(en)
        , pin_a(a)
        , pin_b(b)
        , pin_z(z)
        , state{0xff,0xff,0xff,0xff}
        , statep(0)
        , position(0)
        , intpin(ip)
        , direction(dir)
    {}
        
    inline void enable()
    {
        GPIO_PinSet(pin_en);
        GPIO_PinInterruptEnable(intpin?pin_b:pin_a);
    }
    
    inline void disable()
    {
        GPIO_PinClear(pin_en);
        //GPIO_PinInterruptDisable(pin_a);
        GPIO_PinInterruptDisable(intpin?pin_b:pin_a);
        //GPIO_PinInterruptDisable(pin_z);
    }
    
    void notify();
};

extern std::array<Encoder,4> encoders;


#endif