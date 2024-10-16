#ifndef DRIVERS_PCA9536_H
# define DRIVERS_PCA9536_H

class PCA9536
{
    constexpr static inline uint8_t reg_input = 0;
    constexpr static inline uint8_t reg_output = 1;
    constexpr static inline uint8_t reg_inv = 2;
    constexpr static inline uint8_t reg_dir = 3;
    I2C &i2c;
    uint16_t address;
    volatile uint8_t _value;
    std::mutex lk;
    
    bool _read(uint8_t reg, uint8_t &data)
    {
        reg &= 3;
        if(!i2c.Write(address, &reg, 1))
            return false;
        if(!i2c.Read(address, &data, 1))
            return false;
        return true;
    }
    
    bool _write(uint8_t reg, uint8_t data)
    {
        reg &= 3;
        if(!i2c.Write(address, &reg, 1))
            return false;
        if(!i2c.Write(address, &data, 1))
            return false;
        return true;
    }
    
public:
    
    PCA9536(I2C &i, uint16_t a)
        : i2c(i)
        , address(a)
        , _value(0)
    {
        _write(reg_dir,0);
        _write(reg_output,0);
    }
        
    uint16_t value()
    {
        std::unique_lock ul(lk);
        return _value;
    }

	bool value(uint16_t pins)
    {
        std::unique_lock ul(lk);
        if(_write(reg_output,pins))
        {
            _value = pins;
            return true;
        }
        return false;
    }

	bool pin_value(uint8_t pin)
    {
        std::unique_lock ul(lk);
        return (_value >> pin) & 1;
    }

	bool pin_value(uint8_t pin, bool v)
    {
        std::unique_lock ul(lk);
        uint16_t pins = _value;
        if(v)
            pins |= (1<<pin);
        else
            pins &= ~(1<<pin);
        
        return value(pins);
    }
};
#endif