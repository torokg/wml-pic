#include "../MAX31865.h"
#include <io/host/service.hh>

bool
MAX31865::_read(uint8_t reg, uint8_t &value)
{
    uint8_t rxbuf[2]= {0,0}, txbuf[2] = {reg,0xff};
    std::unique_lock ul(spi.mutex);
    GPIO_PinClear(pin_cs);
    const bool rv = spi.WriteRead(txbuf,2,rxbuf,2);
    GPIO_PinSet(pin_cs);
    if(!rv)
        return false;
    value = rxbuf[1];
    return true;
}

bool
MAX31865::_write(uint8_t reg, uint8_t value)
{
    uint8_t txbuf[2] = {reg|0x80,value};
    std::unique_lock ul(spi.mutex);
    GPIO_PinClear(pin_cs);
    const bool rv = spi.Write(txbuf,2);
    GPIO_PinSet(pin_cs);
    return rv;
}

MAX31865::MAX31865(SPI &s, GPIO_PIN cs)
    : spi(s)
    , pin_cs(cs)
{
    _write(reg_config,0b11010011);
}
    
bool
MAX31865::read(float &result)
{
    union 
    {   uint16_t v;
        uint8_t a[2];
    } v;
    
    if(!_read(reg_rtd_lsb,v.a[0]))
        return false;
    
    if(!_read(reg_rtd_msb,v.a[1]))
        return false;
    
    io::host::service::log("ADC CODE: ",std::hex,v.v);
    result = float(v.v>>1)/32-256;
    return true;
}