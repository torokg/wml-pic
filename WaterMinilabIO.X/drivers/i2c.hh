#ifndef DRIVERS_I2C_HH
# define DRIVERS_I2C_HH
# include <cstddef>
# include <cstdint>
struct I2C
{
    virtual ~I2C() {};
    virtual bool Read(uint16_t address, uint8_t* rdata, size_t rlength) = 0;
    virtual bool Write(uint16_t address, uint8_t* wdata, size_t wlength) = 0;
    virtual bool WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength) = 0;
};

class NativeI2C
    : public I2C
{
    typedef bool(*const read_hnd_t)(uint16_t address, uint8_t* rdata, size_t rlength);
    typedef bool(*const write_hnd_t)(uint16_t address, uint8_t* wdata, size_t wlength);
    typedef bool(*const write_read_hnd_t)(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength);
    
    read_hnd_t _read;
    write_hnd_t _write;
    write_read_hnd_t _write_read;
    
public:
    
    NativeI2C(read_hnd_t r, write_hnd_t w, write_read_hnd_t wr)
        : _read(r)
        , _write(w)
        , _write_read(wr)
    {}
    
    bool Read(uint16_t address, uint8_t* rdata, size_t rlength)
    { return _read(address,rdata,rlength); }
    
    bool Write(uint16_t address, uint8_t* wdata, size_t wlength)
    { return _write(address,wdata,wlength); }
    
    bool WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength)
    { return _write_read(address,wdata,wlength,rdata,rlength); }
};

class DummyI2C
    : public I2C
{
    bool Read(uint16_t address, uint8_t* rdata, size_t rlength)
    { return false; }
    
    bool Write(uint16_t address, uint8_t* wdata, size_t wlength)
    { return false; }
    
    bool WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength)
    { return false; }
};
#endif