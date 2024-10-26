#ifndef DRIVERS_MAX31865_HH
# define DRIVERS_MAX31865_HH
# include <drivers/spi.hh>
class MAX31865
{
    constexpr static uint8_t reg_config = 0x00;
    constexpr static uint8_t reg_rtd_msb = 0x01;
    constexpr static uint8_t reg_rtd_lsb = 0x02;
    constexpr static uint8_t reg_high_fault_msb = 0x03;
    constexpr static uint8_t reg_high_fault_lsb = 0x04;
    constexpr static uint8_t reg_low_fault_msb = 0x05;
    constexpr static uint8_t reg_low_fault_lsb = 0x06;
    constexpr static uint8_t reg_fault_status = 0x7;
    
    SPI &spi;
    GPIO_PIN pin_cs;
    
    bool _read(uint8_t reg, uint8_t &value);
    bool _write(uint8_t reg, uint8_t value);
    
public:
    
    MAX31865(SPI &spi, GPIO_PIN pin_cs);
    
    bool read(float &result);
};
#endif