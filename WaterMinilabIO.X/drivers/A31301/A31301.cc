#include "../A31301.h"

A31301::A31301(I2C &i, uint16_t a)
  : i2c(i)
  , address(a)
{}

bool
A31301::read(result &r)
{
    uint8_t regaddr = 0x1c;
    if(!i2c.WriteRead(address, &regaddr, 1, (uint8_t*)&r, sizeof(result)))
        return false;

    for(uint16_t *p = (uint16_t*)&r; p != (uint16_t*)(&r+1); ++p)
        *p = net::algorithm::hton(*p);        

    r.temp = (int16_t)(((uint16_t)r.temp)<<4)>>4;
    for(uint16_t *p = (uint16_t*)&r.x; p != (uint16_t*)(&r+1); ++p)
        *p = (uint16_t)(((int16_t)(*p<<1))>>1);

    return true;
} 
            