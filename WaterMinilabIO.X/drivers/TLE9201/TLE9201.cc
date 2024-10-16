#include "../TLE9201.h"

void
TLE9201::_write_ctl()
{
    std::unique_lock ul(spi.mutex);
    uint8_t rd = ctl_reg;
    GPIO_PinWrite(pin_cs,0);
    spi.Write(&rd,1);
    GPIO_PinWrite(pin_cs,1);
}

TLE9201::TLE9201(SPI &s, GPIO_PIN cs)
    : spi(s)
    , pin_cs(cs)
    , ctl_reg(0b11101001)
{ _write_ctl(); }


void
TLE9201::start(bool dir)
{
    std::unique_lock ul(lk);
    uint8_t c = ctl_reg;

    if(dir)
        c |= sdir_mask;
    else
        c &= ~sdir_mask;

    c |= sen_mask|spwm_mask;
    ctl_reg = c;
    _write_ctl();
}

void
TLE9201::stop()
{
    std::unique_lock ul(lk);
    ctl_reg &= ~(sen_mask|spwm_mask);
    _write_ctl();
}