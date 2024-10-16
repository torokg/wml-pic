# include "../PCA9546.h"
        
PCA9546::channel::channel(PCA9546 &m, uint8_t l)
    : mux(m)
    , line(l)
{}

bool
PCA9546::channel::Read(uint16_t address, uint8_t* rdata, size_t rlength)
{ return mux.read(line,address,rdata,rlength); }

bool
PCA9546::channel::Write(uint16_t address, uint8_t* wdata, size_t wlength)
{ return mux.read(line,address,wdata,wlength); }

bool
PCA9546::channel::WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength)
{ return mux.write_read(line,address,wdata,wlength,rdata,rlength); }
        
PCA9546::PCA9546(I2C &i, uint16_t a)
    : i2c(i)
    , address(a)
    , channels{ channel{*this,0}, channel{*this,1}, channel{*this,2}, channel{*this,3} }
{}
      
bool
PCA9546::read(uint8_t line, uint16_t address, uint8_t *data, size_t size)
{ return _operate<&I2C::Read>(line,address,data,size); }

bool
PCA9546::write(uint8_t line, uint16_t address, uint8_t *data, size_t size)
{ return _operate<&I2C::Write>(line,address,data,size); }

bool
PCA9546::write_read(uint8_t line, uint16_t address, uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize)
{ return _operate<&I2C::WriteRead>(line,address,wdata,wsize,rdata,rsize); }

