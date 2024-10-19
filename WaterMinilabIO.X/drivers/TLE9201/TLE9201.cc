#include <iomanip>
#include <io/host/service.hh>
#include "../TLE9201.h"

bool
TLE9201::_tx()
{
    uint8_t wr = ctl_reg;
    std::unique_lock ul(spi.mutex);
    GPIO_PinWrite(pin_cs,0);
    bool rv = spi.Write(&wr,1);
    GPIO_PinWrite(pin_cs,1);
    ul.unlock();
    return rv;
}

TLE9201::TLE9201(SPI &s, GPIO_PIN cs)
    : spi(s)
    , pin_cs(cs)
    , ctl_reg(0b11101000)
    , dia_reg(0)
{ _tx(); }


bool
TLE9201::start(bool dir)
{
    std::unique_lock ul(lk);
    uint8_t c = ctl_reg;

    if(dir)
        c |= ctl_sdir_mask;
    else
        c &= ~ctl_sdir_mask;

    c |= ctl_sen_mask|ctl_spwm_mask;
    ctl_reg = c;
    if(!_tx())
        return false;
    
    return true; //!(dia_reg&dia_tv_mask) && (dia_reg&dia_en_mask);
}

bool
TLE9201::stop()
{
    std::unique_lock ul(lk);
    ctl_reg &= ~(ctl_sen_mask|ctl_spwm_mask);
    if(!_tx())
        return false;
    
    return true; //!(dia_reg&dia_tv_mask) && !(dia_reg&dia_en_mask);
}