#ifndef DRIVERS_PCA9546_H
# define DRIVERS_PCA9546_H
# include <mutex>
# include <cstdint>
# include <cstddef>
# include <net/algorithm.h>
# include <drivers/i2c.hh>
# include <io/host/service.hh>

class PCA9546
{
    I2C &i2c;
    const uint16_t address;
    std::mutex lk;
    
    template<auto F, typename... Ts>
    inline bool _operate(uint8_t line, Ts&&... vs)
    {
        if(line > 3)
            return false;
        
        std::unique_lock ul(lk);
        uint8_t cmd = ((1<<line)&0xf);
        if(!i2c.Write(address, &cmd, 1))
            return false;

        bool rv = (i2c.*F)(std::forward<Ts>(vs)...);
        
        cmd = 0;
        i2c.Write(address, &cmd, 1);
        
        return rv;
    }
    
    class channel
        : public I2C
    {
        PCA9546 &mux;
        const volatile uint8_t line;
        
    public:
        
        channel(PCA9546 &m, uint8_t l);
           
        bool Read(uint16_t address, uint8_t* rdata, size_t rlength);
        
        bool Write(uint16_t address, uint8_t* wdata, size_t wlength);
        
        bool WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength);
    };
    
    std::array<channel,4> channels;
    
public:
   
    PCA9546(I2C &i, uint16_t address);
      
    bool read(uint8_t line, uint16_t address, uint8_t *data, size_t size);
    
    bool write(uint8_t line, uint16_t address, uint8_t *data, size_t size);
    
    bool write_read(uint8_t line, uint16_t address, uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize);
    
    inline I2C &operator[](size_t i)
    { return channels[i]; }
    
};
#endif