#ifndef DRIVERS_PCA9536_H
# define DRIVERS_PCA9536_H
# include <mutex>
# include <drivers/i2c.hh>

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
    
    bool _read(uint8_t reg, uint8_t &data);
    
    bool _write(uint8_t reg, uint8_t data);
    
public:
    
    PCA9536(I2C &i, uint16_t a);
        
    uint16_t value();

	bool value(uint16_t pins);

	bool pin_value(uint8_t pin);

	bool pin_value(uint8_t pin, bool v);
    
};
#endif