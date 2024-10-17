#include "../PCA9536.h"
#include <io/host/service.hh>

bool
PCA9536::_read(uint8_t reg, uint8_t &data)
{
    reg &= 3;
    return i2c.WriteRead(address, &reg, 1, &data,1);
}

bool
PCA9536::_write(uint8_t reg, uint8_t data)
{
    uint8_t out[2] = {reg&3,data};
    return i2c.Write(address, out, 2);
}
    
PCA9536::PCA9536(I2C &i, uint16_t a)
    : i2c(i)
    , address(a)
    , _value(0)
{
    if(!_write(reg_dir,0))
    {
        io::host::log("Failed to initialize PCA9536");
        return;
    }
    if(!_write(reg_output,0b1111))
    {
        io::host::log("Failed to initialize PCA9536");
        return;
    }
}
        
uint16_t
PCA9536::value()
{
    std::unique_lock ul(lk);
    return _value;
}

bool
PCA9536::value(uint16_t pins)
{
    std::unique_lock ul(lk);
    if(_write(reg_output,pins))
    {
        _value = pins;
        return true;
    }
    return false;
}

bool
PCA9536::pin_value(uint8_t pin)
{
    std::unique_lock ul(lk);
    return (_value >> pin) & 1;
}

bool
PCA9536::pin_value(uint8_t pin, bool v)
{
    std::unique_lock ul(lk);
    uint16_t pins = _value;
    if(v)
        pins |= (1<<pin);
    else
        pins &= ~(1<<pin);

    return value(pins);
}
