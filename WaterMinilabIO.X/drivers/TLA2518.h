#ifndef DRIVERS_TLA2518_H
# define DRIVERS_TLA2518_H
# include <cstddef>
# include <cstdint>
# include <mutex>
# include <drivers/spi.hh>
# include <io/host/service.hh>
# include "definitions.h"

class TLA2518
{
    constexpr static uint8_t op_write =   0b1000;
    constexpr static uint8_t op_read  =  0b10000;
    constexpr static uint8_t op_set   =  0b11000;
    constexpr static uint8_t op_clear = 0b100000;
    
    constexpr static uint8_t reg_system_status = 0;
    constexpr static uint8_t reg_general_cfg = 1;
    constexpr static uint8_t reg_data_cfg = 2;
    constexpr static uint8_t reg_osr_cfg = 3;
    constexpr static uint8_t reg_opmode_cfg = 4;
    constexpr static uint8_t reg_pin_cfg = 5;
    constexpr static uint8_t reg_gpio_cfg = 7;
    constexpr static uint8_t reg_gpo_drive_cfg = 9;
    constexpr static uint8_t reg_gpo_value = 0xb;
    constexpr static uint8_t reg_gpi_value = 0xd;
    constexpr static uint8_t reg_sequence_cfg = 0x10;
    constexpr static uint8_t reg_channel_sel = 0x11;
    constexpr static uint8_t reg_auto_seq_ch_sel = 0x12;
    
    SPI &spi;
    GPIO_PIN pin_cs;
    
    bool _wrop(uint8_t op, uint8_t address, uint8_t value);
    
    bool _read(uint8_t address,uint8_t &value);
    bool _write(uint8_t address, uint8_t value);
    bool _set(uint8_t address, uint8_t mask);
    bool _clear(uint8_t address, uint8_t mask);

public:
    
    TLA2518(SPI &s, GPIO_PIN pin_cs);
    
    int system_status();
    
    bool data_config(uint8_t v);
    
    bool general_config(uint8_t v);
    
    bool sequence_config(uint8_t v);
    
    bool osr_config(uint8_t v);
    
    bool opmode_config(uint8_t v);
    
    bool channel_select(uint8_t ch);
    
    int channel_select();
    
    bool read(uint16_t &v);
};
#endif