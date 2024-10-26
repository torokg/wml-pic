#include "../TLA2518.h"

bool
TLA2518::_read(uint8_t address,uint8_t &value)
{
    uint8_t txbuf[3] = {0b10000,address,0x0};

    std::unique_lock ul(spi.mutex);
    
    GPIO_PinClear(pin_cs);
    bool rv = spi.Write(txbuf,3);
    GPIO_PinSet(pin_cs);
    
    if(!rv)
        return false;
    
    ul.unlock();
    memset(txbuf,0,3);
    ul.lock();
    GPIO_PinClear(pin_cs);
    rv = spi.WriteRead(txbuf,3, &value, 1);
    GPIO_PinSet(pin_cs);
    return rv;
}

bool
TLA2518::_wrop(uint8_t op, uint8_t address, uint8_t value)
{
    uint8_t txbuf[3] = {op,address,value};
    
    std::unique_lock ul(spi.mutex);
    GPIO_PinClear(pin_cs);
    bool rv = spi.Write(txbuf,3);
    GPIO_PinSet(pin_cs);
    return true;
}

bool
TLA2518::_write(uint8_t address, uint8_t value)
{ return _wrop(op_write, address, value); }

bool
TLA2518::_set(uint8_t address, uint8_t mask)
{ return _wrop(op_set, address,mask); }

bool
TLA2518::_clear(uint8_t address, uint8_t mask)
{ return _wrop(op_clear, address,mask); }

TLA2518::TLA2518(SPI &s, GPIO_PIN cs)
    : spi(s)
    , pin_cs(cs)
{
    GPIO_PinSet(pin_cs);
}

int
TLA2518::system_status()
{
    uint8_t rv = 0;
    if(!this->_read(reg_system_status,rv))
        return -1;
    return rv;
}

    
bool
TLA2518::channel_select(uint8_t ch)
{ return _write(reg_channel_sel,ch&7); }

int
TLA2518::channel_select()
{ 
    uint8_t rv = 0;
    if(!this->_read(reg_channel_sel,rv))
        return -1;
    return rv;
}